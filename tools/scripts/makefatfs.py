#!/usr/bin/env python3
# -*- coding:utf-8 -*-
# SPDX-License-Identifier: GPL-2.0+
#
# Dehuang.Wu
# Copyright (C) 2021-2023 ArtInChip Technology Co., Ltd

import os
import re
import sys
import platform
import argparse
import subprocess

def mkimage_get_resource_size(srcdir, cluster_siz):
    total_size = 0
    root_path =srcdir
    for root, dirs, files in os.walk(srcdir):
        for fn in files:
            fpath = os.path.join(root, fn)
            size = os.path.getsize(fpath)
            size = cluster_siz * int(round((size + cluster_siz - 1) / cluster_siz))
            total_size += size
    return total_size


def mkimage_get_part_size(outfile):
    imgname = os.path.basename(outfile)
    partlist = os.path.join(os.path.dirname(outfile), 'partition_file_list.h')
    size = 0
    if not os.path.exists(partlist):
        return 0
    with open(partlist) as f:
        lines = f.readlines()
        for ln in lines:
            name = ln.split(',')[1].replace('"', '').replace('*', '')
            if imgname == re.sub(".sparse", "", name):
                size = int(ln.split(',')[2])
                return size
    print('Image {} is not used in any partition'.format(imgname))
    print('please check your project\'s image_cfg.json')
    return size


def run_cmd(cmdstr):
    # print(cmdstr)
    cmd = cmdstr.split(' ')
    ret = subprocess.run(cmd, stdout=subprocess.PIPE)
    if ret.returncode != 0:
        sys.exit(1)


def gen_fatfs(tooldir, srcdir, outimg, imgsiz, sector_siz, cluster):
    sector_cnt = int(imgsiz / sector_siz)
    if platform.system() == 'Linux':
        truncate = 'truncate -s {} {}'.format(imgsiz, outimg)
        run_cmd(truncate)

        mformat = '{}mformat -i {} -M {} -T {} -c {}'.format(tooldir, outimg, sector_siz, sector_cnt, cluster)
        run_cmd(mformat)

        mcopy = '{}mcopy -i {} -s {}// ::/'.format(tooldir, outimg, srcdir)
        run_cmd(mcopy)

        # gen sparse format
        img2simg = '{}img2simg {} {}.sparse 1024'.format(tooldir, outimg, outimg)
        run_cmd(img2simg)

    elif platform.system() == 'Windows':
        outimg = outimg.replace(' ', '\\ ')
        outimg = outimg.replace('/', '\\')
        srcdir = srcdir.replace(' ', '\\ ')
        srcdir = srcdir.replace('/', '\\')

        truncate = '{}truncate.exe -s {} {}'.format(tooldir, imgsiz, outimg)
        run_cmd(truncate)

        mformat = '{}mformat.exe -M {} -T {} -c {} -i {}'.format(tooldir, sector_siz, sector_cnt, cluster, outimg)
        run_cmd(mformat)

        mcopy = '{}mcopy.exe -i {} -s {}\\\\ ::/'.format(tooldir, outimg, srcdir)
        run_cmd(mcopy)

        # gen sparse format
        img2simg = '{}img2simg.exe {} {}.sparse 1024'.format(tooldir, outimg, outimg)
        run_cmd(img2simg)


def round_pow2(x):
    cnt = 0
    shift = 64
    last_one = -1
    for i in range(64):
        if (x >> i) & 0x1:
            last_one = i
            cnt += 1
    if last_one < 0:
        return 0

    if cnt > 1:
        last_one += 1
    value = 1 << last_one
    return value


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-a", "--auto", action='store_true',
                        help="auto size of FAT image")
    parser.add_argument("-f", "--fullpart", action='store_true',
                        help="image size is partition size")
    parser.add_argument("-i", "--inputdir", type=str,
                        help="input directory")
    parser.add_argument("-o", "--outfile", type=str,
                        help="output file")
    parser.add_argument("-g", "--imgsize", type=str,
                        help="image size")
    parser.add_argument("-s", "--sector", type=str,
                        help="sector size")
    parser.add_argument("-c", "--cluster", type=str,
                        help="cluster size")
    parser.add_argument("-t", "--tooldir", type=str,
                        help="tool directory")
    parser.add_argument("-r", "--raw", action='store_true',
                        help="Don't strip FAT image, keep raw image")
    args = parser.parse_args()

    cluster = int(args.cluster)
    # cluster should be pow of 2
    cluster = round_pow2(cluster)
    sector_siz = int(args.sector)
    calc_mini = True
    strip_siz = True
    if args.auto:
        # No need to strip size in auto mode
        strip_siz = False
    if args.raw:
        # No need to strip size if user select raw image
        strip_siz = False
    if calc_mini:
        cluster_siz = cluster * sector_siz
        data_siz = mkimage_get_resource_size(args.inputdir, cluster_siz)
        # Size should alignment with cluster size
        data_siz = cluster_siz * int(((data_siz + cluster_siz - 1) / cluster_siz))
    if args.auto:
        data_clus_cnt = 2 + data_siz / cluster_siz
        data_region_sz = data_clus_cnt * cluster_siz
        if data_clus_cnt < 4096:
            # FAT12
            # - BPB_RsvdSecCnt should be 1
            # - BPB_RootEntCnt max 512
            # - FATsz: 2 * FAT
            # - DATA Region: 2 cluster + DATA
            fat_siz = (data_region_sz / cluster_siz) * 12 / 8
            rsvd_siz = 1 * sector_siz
            root_ent_cnt = 512 * 32
        elif data_clus_cnt < 65525:
            # FAT16
            # - BPB_RsvdSecCnt should be 1
            # - BPB_RootEntCnt max 512
            # - FATsz: 2 * FAT
            # - DATA Region: 2 cluster + DATA
            fat_siz = (data_region_sz / cluster_siz) * 16 / 8
            rsvd_siz = 1 * sector_siz
            root_ent_cnt = 512 * 32
            imgsiz = rsvd_siz + 2 * fat_siz + root_ent_cnt + data_region_sz
        else:
            # FAT32
            # - BPB_RsvdSecCnt fixed 32
            # - BPB_RootEntCnt fixed 0
            # - FATsz: 2 * FAT
            # - DATA Region: 2 cluster + DATA
            fat_siz = data_region_sz / cluster_siz * 4
            rsvd_siz = 32 * sector_siz
            root_ent_cnt = 0
        fat_siz = sector_siz * int((fat_siz + sector_siz - 1) / sector_siz)
        imgsiz = rsvd_siz + 2 * fat_siz + root_ent_cnt + data_region_sz
        # Round to cluster alignment
        imgsiz = cluster_siz * int(((imgsiz + cluster_siz - 1) / cluster_siz))
    elif args.fullpart:
        imgsiz = mkimage_get_part_size(args.outfile)
    else:
        imgsiz = int(args.imgsize)

    gen_fatfs(args.tooldir, args.inputdir, args.outfile, imgsiz, sector_siz, cluster)
    if strip_siz:
        clus_cnt = imgsiz / cluster_siz
        if clus_cnt < 65536:
            # FAT16/FAT12, assume it is FAT16, and evaluate the valid data size
            fat_siz = (clus_cnt * 16) / 8
            rsvd_siz = 1 * sector_siz
            root_ent_cnt = 512 * 32
        else:
            # FAT32, evaluate the valid data size
            fat_siz = (clus_cnt * 32) / 8
            rsvd_siz = 32 * sector_siz
            root_ent_cnt = 0
        minimal_siz = rsvd_siz + 2 * fat_siz + root_ent_cnt + 2 * cluster_siz + data_siz
        # Round to cluster alignment
        minimal_siz = cluster_siz * int(((minimal_siz + cluster_siz - 1) / cluster_siz))
        if platform.system() == 'Linux':
            truncate = 'truncate -s {} {}'.format(minimal_siz, args.outfile)
            run_cmd(truncate)
        elif platform.system() == 'Windows':
            truncate = '{}truncate.exe -s {} {}'.format(args.tooldir, minimal_siz, args.outfile)
            run_cmd(truncate)
