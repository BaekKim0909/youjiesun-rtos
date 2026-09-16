//
// Created by 74222 on 2026/9/16.
//

#include "../Inc/record_outcome_page.h"

#include <math.h>

#include "mainUI.h"

LV_FONT_DECLARE(chinese_character_20)

void load_one_fill_outcome_page(const test_record_t record)
{
    current_page_index_g = OUTCOME_ONE_FILL_PAGE;
    lv_obj_t *container = container_get();

    lv_obj_t *table = lv_table_create(container);

    lv_table_set_column_count(table, 2);
    lv_obj_set_pos(table, 10, 10);
    lv_obj_set_size(table, 625, 420);
    lv_obj_remove_flag(table, LV_OBJ_FLAG_SCROLLABLE);
    lv_table_set_column_width(table, 0, 205);
    lv_table_set_column_width(table, 1, 420);

    lv_obj_set_style_pad_all(table, 0, LV_PART_ITEMS);
    lv_obj_set_style_pad_top(table, 7, LV_PART_ITEMS);
    lv_obj_set_style_pad_bottom(table, 7, LV_PART_ITEMS);
    lv_obj_set_style_border_width(table, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(table, 1, LV_PART_ITEMS);
    lv_obj_set_style_border_color(table, lv_color_hex(0xFFFFFF), LV_PART_ITEMS);
    lv_obj_set_style_border_side(
        table, LV_BORDER_SIDE_RIGHT | LV_BORDER_SIDE_BOTTOM | LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_TOP, LV_PART_ITEMS);
    lv_obj_set_style_bg_color(table, lv_color_hex(0x123867), LV_STATE_DEFAULT | LV_PART_ITEMS);
    lv_obj_set_style_bg_color(table, lv_color_hex(0x123867), LV_STATE_DEFAULT | LV_PART_MAIN);
    lv_obj_set_style_text_font(table, &chinese_character_20, LV_STATE_DEFAULT | LV_PART_ITEMS);
    lv_obj_set_style_text_align(table, LV_TEXT_ALIGN_CENTER, LV_STATE_DEFAULT | LV_PART_ITEMS);
    lv_obj_set_style_text_color(table, lv_color_hex(0xFFFFFF), LV_STATE_DEFAULT | LV_PART_ITEMS);

    if (record.standard_template == NO_DC_TEMPLATE || (
            record.standard_template == FULL_TEMPLATE && record.rho_param == 0))
    {
        /* code */
        lv_table_set_row_count(table, 7);


        /* code */
        lv_table_set_cell_value(table, 0, 0, lv_translation_get("sample"));
        lv_table_set_cell_value(table, 1, 0, lv_translation_get("test_standard"));
        lv_table_set_cell_value(table, 2, 0, lv_translation_get("electrode_info"));
        lv_table_set_cell_value(table, 3, 0, lv_translation_get("ac_param"));
        lv_table_set_cell_value(table, 4, 0, lv_translation_get("test_temperature"));
        lv_table_set_cell_value(table, 5, 0, "ε");
        lv_table_set_cell_value_fmt(table, 6, 0, "tanδ(%dHz)", record.frequency);

        lv_table_set_cell_value_fmt(table, 2, 1, "%s:  %s,  %.2fpF",
                                    lv_translation_get("electrode"),
                                    record.electrode_name,
                                    record.capacitance);

        lv_table_set_cell_value(table, 0, 1, record.sample_name);
        lv_table_set_cell_value(table, 1, 1, record.standard_name);
        lv_table_set_cell_value_fmt(table, 3, 1, "%.1fV/mm    %.1fV", record.ac_voltage_1 / 2.0f,
                                    record.ac_voltage_1);
        lv_table_set_cell_value_fmt(table, 4, 1, "%.1f℃", record.temperature_1);
        lv_table_set_cell_value_fmt(table, 5, 1, "%.2f", record.permittivity_1);
        lv_table_set_cell_value_fmt(table, 6, 1, "%.6f", record.dielectric_loss_1);
    }
    else if (record.standard_template == FULL_TEMPLATE && record.rho_param == 1)
    {
        /* code */
        lv_table_set_row_count(table, 9);
        /* code */
        lv_table_set_cell_value(table, 0, 0, lv_translation_get("sample"));
        lv_table_set_cell_value(table, 1, 0, lv_translation_get("test_standard"));
        lv_table_set_cell_value(table, 2, 0, lv_translation_get("electrode_info"));
        lv_table_set_cell_value(table, 3, 0, lv_translation_get("ac_param"));
        lv_table_set_cell_value(table, 4, 0, lv_translation_get("dc_param"));
        lv_table_set_cell_value(table, 5, 0, lv_translation_get("test_temperature"));
        lv_table_set_cell_value(table, 6, 0, "ε");
        lv_table_set_cell_value_fmt(table, 7, 0, "tanδ(%dHz)", record.frequency);
        lv_table_set_cell_value(table, 8, 0, "Rho+(Ωm)");
        lv_table_set_cell_value_fmt(table, 2, 1, "%s:  %s,  %.2fpF",
                                    lv_translation_get("electrode"),
                                    record.electrode_name,
                                    record.capacitance);


        lv_table_set_cell_value(table, 0, 1, record.sample_name);
        lv_table_set_cell_value(table, 1, 1, record.standard_name);
        lv_table_set_cell_value_fmt(table, 3, 1, "%.1fV/mm    %.1fV", record.ac_voltage_1 / 2.0f,
                                    record.ac_voltage_1);
        lv_table_set_cell_value_fmt(table, 4, 1, "%.1fV/mm    %.1fV", record.dc_voltage_1 / 2.0f,
                                    record.dc_voltage_1);
        lv_table_set_cell_value_fmt(table, 5, 1, "%.1f℃", record.temperature_1);
        lv_table_set_cell_value_fmt(table, 6, 1, "%.2f", record.permittivity_1);
        lv_table_set_cell_value_fmt(table, 7, 1, "%.6f", record.dielectric_loss_1);
        lv_table_set_cell_value_fmt(table, 8, 1, "%.5E", record.rho_pos_1);
    }
    else if (record.standard_template == FULL_TEMPLATE && record.rho_param == 2)
    {
        /* code */
        lv_table_set_row_count(table, 11);


        /* code */
        lv_table_set_cell_value(table, 0, 0, lv_translation_get("sample"));
        lv_table_set_cell_value(table, 1, 0, lv_translation_get("test_standard"));
        lv_table_set_cell_value(table, 2, 0, lv_translation_get("electrode_info"));
        lv_table_set_cell_value(table, 3, 0, lv_translation_get("ac_param"));
        lv_table_set_cell_value(table, 4, 0, lv_translation_get("dc_param"));
        lv_table_set_cell_value(table, 5, 0, lv_translation_get("test_temperature"));
        lv_table_set_cell_value(table, 6, 0, "ε");
        lv_table_set_cell_value_fmt(table, 7, 0, "tanδ(%dHz)", record.frequency);
        lv_table_set_cell_value(table, 8, 0, "Rho+(Ωm)");
        lv_table_set_cell_value(table, 9, 0, "Rho-(Ωm)");
        lv_table_set_cell_value(table, 10, 0, "测试结果");
        lv_table_set_cell_value_fmt(table, 2, 1, "%s:  %s,  %.2fpF",
                                    lv_translation_get("electrode"),
                                    record.electrode_name,
                                    record.capacitance);

        lv_table_set_cell_value(table, 0, 1, record.sample_name);
        lv_table_set_cell_value(table, 1, 1, record.standard_name);
        lv_table_set_cell_value_fmt(table, 3, 1, "%.1fV/mm    %.1fV", record.ac_voltage_1 / 2.0f,
                                    record.ac_voltage_1);
        lv_table_set_cell_value_fmt(table, 4, 1, "%.1fV/mm    %.1fV", record.dc_voltage_1 / 2.0f,
                                    record.dc_voltage_1);
        lv_table_set_cell_value_fmt(table, 5, 1, "%.1f℃", record.temperature_1);
        lv_table_set_cell_value_fmt(table, 6, 1, "%.2f", record.permittivity_1);
        lv_table_set_cell_value_fmt(table, 7, 1, "%.6f", record.dielectric_loss_1);
        lv_table_set_cell_value_fmt(table, 8, 1, "%.5E", record.rho_pos_1);
        lv_table_set_cell_value_fmt(table, 9, 1, "%.5E", record.rho_neg_1);

        // 体积电阻率1
        float abs_Resistivity1Pos = fabsf(record.rho_pos_1);
        float abs_Resistivity1Neg = fabsf(record.rho_neg_1);

        float max_abs_Resistivity1_val = fmaxf(abs_Resistivity1Pos, abs_Resistivity1Neg);
        float min_abs_Resistivity1_val = fminf(abs_Resistivity1Pos, abs_Resistivity1Neg);

        float deviation_Resistivity1 = 0.0f; //  体积电阻率1偏差

        if (max_abs_Resistivity1_val == 0.0f)
        {
            /* code */
            deviation_Resistivity1 = 100.0f; // 赋一个无效值
        }
        else
        {
            deviation_Resistivity1 = (max_abs_Resistivity1_val - min_abs_Resistivity1_val) / max_abs_Resistivity1_val *
                                     100.0f;
        }
        if (deviation_Resistivity1 <= 35.0f)
        {
            /* code */
            lv_table_set_cell_value(table, 10, 1, lv_translation_get("valid"));
        }
        else
        {
            lv_table_set_cell_value(table, 10, 1, lv_translation_get("invalid"));
        }
    }
    else if (record.standard_template == NO_AC_TEMPLATE)
    {
        if (record.rho_param == 2)
        {
            /* code */
            lv_table_set_row_count(table, 8);


            /* code */
            lv_table_set_cell_value(table, 0, 0, lv_translation_get("sample"));
            lv_table_set_cell_value(table, 1, 0, lv_translation_get("test_standard"));
            lv_table_set_cell_value(table, 2, 0, lv_translation_get("electrode_info"));
            lv_table_set_cell_value(table, 3, 0, lv_translation_get("dc_param"));
            lv_table_set_cell_value(table, 4, 0, lv_translation_get("test_temperature"));
            lv_table_set_cell_value(table, 5, 0, "Rho+(Ωm)");
            lv_table_set_cell_value(table, 6, 0, "Rho-(Ωm)");
            lv_table_set_cell_value(table, 7, 0, "测试结果");
            lv_table_set_cell_value_fmt(table, 2, 1, "%s:  %s,  %.2fpF", lv_translation_get("electrode"),
                                        record.electrode_name, record.capacitance);


            lv_table_set_cell_value(table, 0, 1, record.sample_name);
            lv_table_set_cell_value(table, 1, 1, record.standard_name);
            lv_table_set_cell_value_fmt(table, 3, 1, "%.1fV/mm    %.1fV", record.dc_voltage_1 / 2.0f,
                                        record.dc_voltage_1);
            lv_table_set_cell_value_fmt(table, 4, 1, "%.1f℃", record.temperature_1);
            lv_table_set_cell_value_fmt(table, 5, 1, "%.5E", record.rho_pos_1);
            lv_table_set_cell_value_fmt(table, 6, 1, "%.5E", record.rho_neg_1);

            // 体积电阻率1
            float abs_Resistivity1Pos = fabsf(record.rho_pos_1);
            float abs_Resistivity1Neg = fabsf(record.rho_neg_1);

            float max_abs_Resistivity1_val = fmaxf(abs_Resistivity1Pos, abs_Resistivity1Neg);
            float min_abs_Resistivity1_val = fminf(abs_Resistivity1Pos, abs_Resistivity1Neg);

            float deviation_Resistivity1 = 0.0f; //  体积电阻率1偏差

            if (max_abs_Resistivity1_val == 0.0f)
            {
                /* code */
                deviation_Resistivity1 = 100.0f; // 赋一个无效值
            }
            else
            {
                deviation_Resistivity1 = (max_abs_Resistivity1_val - min_abs_Resistivity1_val) /
                                         max_abs_Resistivity1_val * 100.0f;
            }
            if (deviation_Resistivity1 <= 35.0f)
            {
                /* code */
                lv_table_set_cell_value(table, 10, 1, lv_translation_get("valid"));
            }
            else
            {
                lv_table_set_cell_value(table, 10, 1, lv_translation_get("invalid"));
            }
        }
        else if (record.rho_param == 1)
        {
            /* code */
            lv_table_set_row_count(table, 6);

            /* code */
            lv_table_set_cell_value(table, 0, 0, lv_translation_get("sample"));
            lv_table_set_cell_value(table, 1, 0, lv_translation_get("test_standard"));
            lv_table_set_cell_value(table, 2, 0, lv_translation_get("electrode_info"));
            lv_table_set_cell_value(table, 3, 0, lv_translation_get("dc_param"));
            lv_table_set_cell_value(table, 4, 0, lv_translation_get("test_temperature"));
            lv_table_set_cell_value(table, 5, 0, "Rho+(Ωm)");
            lv_table_set_cell_value_fmt(table, 2, 1, "%s:  %s,  %.2fpF",
                                        lv_translation_get("electrode"),
                                        record.electrode_name,
                                        record.capacitance);
            lv_table_set_cell_value(table, 0, 1, record.sample_name);
            lv_table_set_cell_value(table, 1, 1, record.standard_name);
            lv_table_set_cell_value_fmt(table, 3, 1, "%.1fV/mm    %.1fV", record.dc_voltage_1 / 2.0f,
                                        record.dc_voltage_1);
            lv_table_set_cell_value_fmt(table, 4, 1, "%.1f℃", record.temperature_1);
            lv_table_set_cell_value_fmt(table, 5, 1, "%.5E", record.rho_pos_1);
        }
    }
}
