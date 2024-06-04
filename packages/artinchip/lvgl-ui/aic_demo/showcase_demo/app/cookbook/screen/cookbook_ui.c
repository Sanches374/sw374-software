/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  ArtInChip
 */

#include "cookbook_comp.h"
#include "showcase_demo.h"
#include "custom_list_ui.h"
#include "string.h"

static void cookbook_data_init(void);

static const char* additional_info = "Welcome to the recipe demo "
"provided by #ffbb00 ArTinChip #";

static const char* rice_noodle_introduce = "LiuShi Rice Noodles is a "
"distinctive traditional specialty snack "
"originating from Liuzhou, Guangxi Province, China, and is renowned "
"for its unique sour and spicy flavor. Key features include rice "
"noodles served with a broth made by simmering river snails and "
"a variety of accompaniments such as sour bamboo shoots, black "
"fungus, peanuts, pickled beans, and fried tofu skin.";

static const char *rice_noodle_cook_steps = "1: Prep the Rice Noodles: "
"Soak dried rice noodles in cold water until soft, then cook "
"according to package instructions until al dente or fully "
"cooked, rinse under cold water, and set aside.\n\n"
"2: Prepare the River Snail Soup: Clean fresh river snails "
"and lightly saute until golden; add water, ginger, star "
"anise, cinnamon bark, and bay leaves to simmer for several "
"hours. Remove snail meat, strain the broth, season it with salt and sugar.\n\n"
"3: Prepare Accompaniments: Slice sour bamboo shoots, cut tofu "
"skin into small pieces, soak and wash black fungus and "
"daylilies, blanch greens, and crush fried peanuts.\n\n"
"4: Assemble the LiuShi Rice Noodles: Place cooked "
"rice noodles in a bowl, pour over hot river snail "
"broth, then arrange sour bamboo shoots, tofu skin, "
"black fungus, daylilies, greens, and crushed peanuts on top. \n\n"
"5: Seasoning: Add chili oil, special LiuZhou sauce, light soy sauce, "
"and vinegar to taste, adjusting the level of sourness and spiciness "
"according to personal preference.\n\n"
"6: Completion: Gently mix everything together so that the noodles "
"absorb the flavors of the broth, and serve a bowl of authentic "
"LiuShi Rice Noodles.";

static const char *rice_noodle_note = "Note: Please note that this is a simplified "
"version of making LiuShi Rice Noodles, and actual preparation may "
"involve more intricate details and regional-specific cooking "
"techniques. The essence of the dish lies in the slow-cooked river "
"snail broth, which often involves unique recipes and secret methods to enhance the flavor.";

static const char *noodle_introduce = "Braised Beef Noodle Soup Braised Beef Noodle "
"Soup is a classic Chinese comfort food originating from various regions across China, "
"most notably associated with Lanzhou Beef Noodles, Xuhui-style Beef Noodles, and "
"Sichuan Beef Noodles. It's characterized by tender braised beef, chewy noodles, and"
"a rich, savory broth that is both deeply flavorful and aromatic.m Liuzhou, Guangxi "
"Province, China, and is renowned for its unique sour and spicy flavor. Key features "
"include rice noodles served with a broth made by simmering river snails and a variety "
"of accompaniments such as sour bamboo shoots, black fungus, peanuts, pickled beans, "
"and fried tofu skin.";

static const char *noodle_cook_steps = "1: Cut the beef into large chunks, blanch "
"in boiling water to remove impurities, drain and set aside.\n\n"
"2: Heat oil in a pot, melt the brown sugar until it caramelizes and turns red. "
"Add the beef chunks and sear until well-browned.\n\n"
"3: Add sliced spring onions, ginger slices, star anise, and bay leaf to the pot, "
"saute until fragrant. Deglaze with cooking wine and season with light and dark soy sauces.\n\n"
"4: Pour in enough water to cover the beef, bring to a boil, then lower the heat to "
"a simmer and braise for about 1.5 to 2 hours until the beef is tender.\n\n"
"5: While braising the beef, cook noodles in another pot according to package "
"instructions until done. Rinse under cold water and drain.\n\n"
"6: Once the beef is tender, adjust the seasoning with salt. Thicken the soup "
"with starch if a thicker consistency is desired.\n\n"
"7: Place cooked noodles in individual serving bowls, ladle the braised beef and "
"its rich sauce over the noodles. Garnish with chopped green scallions or "
"cilantro before serving.";

static const char *noodle_note = "Note: This recipe provides a general guideline for "
"preparing homemade Braised Beef Noodle Soup. Regional variations might call for "
"specific ingredients or techniques, but this version captures the essence of the "
"dish's rich and hearty flavors. Adjust the spice level and sweetness according to "
"your taste preferences.";

static void ui_cookbook_cb(lv_event_t *e);

static cookbook_data_t *data_rice_noodle;
static cookbook_data_t *data_noodle;

lv_obj_t *cookbook_ui_init(void)
{
    if (data_rice_noodle == NULL) {
        data_rice_noodle = (cookbook_data_t *)lv_mem_alloc(sizeof(cookbook_data_t));
        data_noodle = (cookbook_data_t *)lv_mem_alloc(sizeof(cookbook_data_t));
        cookbook_data_init();
    }

    lv_obj_t *cookbook_ui = lv_obj_create(NULL);
    lv_obj_set_style_opa(cookbook_ui, LV_OPA_0, 0);
    lv_obj_set_size(cookbook_ui, LV_HOR_RES * 2, LV_VER_RES);
    lv_obj_set_scrollbar_mode(cookbook_ui, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_snap_x(cookbook_ui, LV_SCROLL_SNAP_START);

    lv_obj_t *cookbook_liushi = cookbook_comp_create(cookbook_ui);
    cookbook_comp_set_data(cookbook_liushi, data_rice_noodle);
    lv_obj_set_align(cookbook_liushi, LV_ALIGN_TOP_LEFT);
    lv_obj_set_pos(cookbook_liushi, 0, 0);

    lv_obj_t *cookbook_noodle = cookbook_comp_create(cookbook_ui);
    cookbook_comp_set_data(cookbook_noodle, data_noodle);
    lv_obj_set_align(cookbook_noodle, LV_ALIGN_TOP_LEFT);
    lv_obj_set_pos(cookbook_noodle, LV_HOR_RES, 0);

    lv_obj_add_event_cb(cookbook_ui, ui_cookbook_cb, LV_EVENT_ALL, NULL);
    return cookbook_ui;
}

static void ui_cookbook_cb(lv_event_t *e)
{
    lv_event_code_t code = (lv_event_code_t)lv_event_get_code(e);
    if (code == LV_EVENT_SCREEN_UNLOAD_START) {;}
    if (code == LV_EVENT_SCREEN_UNLOADED) {;}
    if (code == LV_EVENT_DELETE) {
        lv_mem_free(data_rice_noodle);
        lv_mem_free(data_noodle);
        data_rice_noodle = NULL;
        data_noodle = NULL;
    }
    if (code == LV_EVENT_SCREEN_LOADED) {;}
}

static void cookbook_data_init(void)
{
    memset(data_rice_noodle, 0, sizeof(cookbook_data_t));
    /* head */
    strcpy(data_rice_noodle->cookbook_title, LVGL_PATH(cookbook/cook_menu.png));
    strcpy(data_rice_noodle->food_path, LVGL_PATH(cookbook/liushi_noodles_img.png));
    strcpy(data_rice_noodle->additional_info, additional_info);

    /* introduce */
    strcpy(data_rice_noodle->food_name, LVGL_PATH(cookbook/liushi_noodles_name.png));
    strcpy(data_rice_noodle->food_introduction, rice_noodle_introduce);

    /* ingredients */
    strcpy(data_rice_noodle->main_ingredients[0], "Dried Rice Noodles");
    strcpy(data_rice_noodle->main_ingredients_weight[0], "200g");
    strcpy(data_rice_noodle->main_ingredients[1], "River Snail");
    strcpy(data_rice_noodle->main_ingredients_weight[1], " ");

    strcpy(data_rice_noodle->supporting_ingredients[0], "Sour Bamboo Shoots");
    strcpy(data_rice_noodle->supporting_ingredients_weight[0], "50g");
    strcpy(data_rice_noodle->supporting_ingredients[1], "Fried Tofu Skin");
    strcpy(data_rice_noodle->supporting_ingredients_weight[1], "30g");
    strcpy(data_rice_noodle->supporting_ingredients[2], "Pickled Bean Curd");
    strcpy(data_rice_noodle->supporting_ingredients_weight[2], " ");
    strcpy(data_rice_noodle->supporting_ingredients[3], "Black Fungus");
    strcpy(data_rice_noodle->supporting_ingredients_weight[3], " ");
    strcpy(data_rice_noodle->supporting_ingredients[4], "Daylily Bulbs");
    strcpy(data_rice_noodle->supporting_ingredients_weight[4], " ");
    strcpy(data_rice_noodle->supporting_ingredients[5], "Fried Peanuts");
    strcpy(data_rice_noodle->supporting_ingredients_weight[5], " ");
    strcpy(data_rice_noodle->supporting_ingredients[6], "Blanched Greens");
    strcpy(data_rice_noodle->supporting_ingredients_weight[6], " ");
    strcpy(data_rice_noodle->supporting_ingredients[7], "Chili Oil");
    strcpy(data_rice_noodle->supporting_ingredients_weight[7], " ");
    strcpy(data_rice_noodle->supporting_ingredients[8], "Special LiuZhou Sauce ");
    strcpy(data_rice_noodle->supporting_ingredients_weight[8], " ");

    /* cooking steps */
    strcpy(data_rice_noodle->cooking_steps, rice_noodle_cook_steps);
    strcpy(data_rice_noodle->note, rice_noodle_note);

    memset(data_noodle, 0, sizeof(cookbook_data_t));
    /* head */
    strcpy(data_noodle->cookbook_title, LVGL_PATH(cookbook/cook_menu.png));
    strcpy(data_noodle->food_path, LVGL_PATH(cookbook/braised_beed_img.png));
    strcpy(data_noodle->additional_info, additional_info);

    /* introduce */
    strcpy(data_noodle->food_name, LVGL_PATH(cookbook/braised_beef_name.png));
    strcpy(data_noodle->food_introduction, noodle_introduce);

    /* ingredients */
    strcpy(data_noodle->main_ingredients[0], "Beef Brisket");
    strcpy(data_noodle->main_ingredients_weight[0], "1000g");
    strcpy(data_noodle->main_ingredients[1], "Noodles");
    strcpy(data_noodle->main_ingredients_weight[1], "appropriate amount");

    strcpy(data_noodle->supporting_ingredients[0], "Spring Onions");
    strcpy(data_noodle->supporting_ingredients_weight[0], "2 stalks");
    strcpy(data_noodle->supporting_ingredients[1], "Ginger Slices");
    strcpy(data_noodle->supporting_ingredients_weight[1], "3-4 pieces");
    strcpy(data_noodle->supporting_ingredients[2], "Star Anise");
    strcpy(data_noodle->supporting_ingredients_weight[2], "2 pods");
    strcpy(data_noodle->supporting_ingredients[3], "Bay Leaf");
    strcpy(data_noodle->supporting_ingredients_weight[3], "1 piece");
    strcpy(data_noodle->supporting_ingredients[4], "Cooking Wine");
    strcpy(data_noodle->supporting_ingredients_weight[4], "as needed");
    strcpy(data_noodle->supporting_ingredients[5], "Light Soy Sauce");
    strcpy(data_noodle->supporting_ingredients_weight[5], "as needed");
    strcpy(data_noodle->supporting_ingredients[6], "Brown Sugar");
    strcpy(data_noodle->supporting_ingredients_weight[6], "30g");
    strcpy(data_noodle->supporting_ingredients[7], "Water");
    strcpy(data_noodle->supporting_ingredients_weight[7], "as needed");
    strcpy(data_noodle->supporting_ingredients[8], "Salt");
    strcpy(data_noodle->supporting_ingredients_weight[8], "as needed");
    strcpy(data_noodle->supporting_ingredients[8], "Green Scallions");
    strcpy(data_noodle->supporting_ingredients_weight[8], "(optional)");

    /* cooking steps */
    strcpy(data_noodle->cooking_steps, noodle_cook_steps);
    strcpy(data_noodle->note, noodle_note);
}
