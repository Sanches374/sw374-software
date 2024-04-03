# QC 测试

此说明文档仅用于AIC 内部 QC 测试环境配置文档

## 1. 环境配置

lunch d13x_qc88-nor_rt-thread_helloworld_defconfig

修改: ./SConscript 的 use_qc_demo为True
```py
use_qc_demo = True
#use_qc_demo = False
```

定义宏：../../aic_ui.c 的 AIC_LVGL_QC_TEST_DEMO
```c
#define AIC_LVGL_QC_TEST_DEMO
```

## 2. 测试方法

核心的测试方法是：
-   通过函数接口 msh_exec 执行导出到控制台的测试命令，得到返回值，判断测试结果
-   直接调用相关的测试函数，得到返回值，判断测试结果

```c
/*  核心函数接口 */
int qc_module_add_cmd(struct qc_module *module, int exec_set, char *cmd, int exec_record);
int qc_module_add_func(struct qc_module *module, int exec_set, qc_exec_func func, void *func_para, int exec_record);
int qc_module_execute(struct qc_module *module, int exec_set, int exec_flag);
```
基本运行流程如下：
```c
/*
测试线程： 执行测试函数或者命令
                  ↑ |
                  | |
                  | |
 LVGL 推送测试信息 | | 测试线程推送测试结果
                  | |
                  | |
                  | ↓
LVGL线程： UI更新和测试开启控制
*/
```
