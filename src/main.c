#include <stdint.h>
#include "vram.h"
#include "smd/megadrive.h"
#include "../res/res.h"

int main()
{
    uint8_t color = 1;
 
    smd_ints_enable();
    
    tiles_load_fast(res_font_sys, VRAM_INDEX_FONT, RES_FONT_SYS_SIZE);
    tiles_load_fast(res_img_skel_tileset, 1, RES_IMG_SKEL_TILESET_SIZE);

    pal_primary_set(PAL_0_INDEX, RES_PAL_PLAYER_SIZE, res_pal_player);
    pal_primary_set(PAL_1_INDEX, RES_PAL_COLLECTIBLES_SIZE, res_pal_collectibles);            
    pal_primary_set(PAL_2_INDEX, RES_PAL_ENEMY00_SIZE, res_pal_enemy00);
    pal_primary_set(PAL_3_INDEX, RES_PAL_ENEMY01_SIZE, res_pal_enemy01);   


    uint16_t text[41];
    uint16_t size;

    text_font_set(VRAM_INDEX_FONT);
    text_pal_set(PAL_1);
    size = text_render("IT WORKS!", text);
    plane_hline_draw(PLANE_A, text, 2, 2, size, false);
    size = text_render("FINALLY IT WRITES TEXT ON SCREEN!!", text);
    plane_hline_draw(PLANE_A, text, 2, 4, size, false);

    text_pal_set(PAL_0);
    text_nrender("THIS", text + 18, 4);
    plane_hline_draw(PLANE_A, text, 2, 6, size, false);
    
    vid_display_enable();
    while (1)
    {
        pad_update();

        /* Check press button  */
        if (pad_btn_pressed(PAD_1, PAD_BTN_A))
        {
            //vid_display_disable();
            pal_primary_set(PAL_0_INDEX, RES_PAL_PLAYER_SIZE, res_pal_player);
            pal_primary_set(PAL_1_INDEX, RES_PAL_COLLECTIBLES_SIZE, res_pal_collectibles);            
            pal_primary_set(PAL_2_INDEX, RES_PAL_ENEMY00_SIZE, res_pal_enemy00);
            pal_primary_set(PAL_3_INDEX, RES_PAL_ENEMY01_SIZE, res_pal_enemy01);   
            pal_alternate_set(PAL_0_INDEX, RES_PAL_COLLECTIBLES2_SIZE, res_pal_collectibles2);
            pal_alternate_set(PAL_1_INDEX, RES_PAL_COLLECTIBLES2_SIZE, res_pal_collectibles2);          
            pal_alternate_set(PAL_2_INDEX, RES_PAL_COLLECTIBLES2_SIZE, res_pal_collectibles2);
            pal_alternate_set(PAL_3_INDEX, RES_PAL_COLLECTIBLES2_SIZE, res_pal_collectibles2);
        }
        /* Check press button  */        
        if (pad_btn_pressed(PAD_1, PAD_BTN_B))
        {
            plane_clear(PLANE_A);
            plane_clear(PLANE_B);
        }
        if (pad_btn_pressed(PAD_1, PAD_BTN_C))
        {
            vid_background_color_set(color);
            ++color;
            //if (!pal_is_fading())
            {
                pal_fade(3);
                pal_fade_wait();
            pal_primary_set(PAL_0_INDEX, RES_PAL_PLAYER_SIZE, res_pal_player);
            pal_primary_set(PAL_1_INDEX, RES_PAL_COLLECTIBLES_SIZE, res_pal_collectibles);            
            pal_primary_set(PAL_2_INDEX, RES_PAL_ENEMY00_SIZE, res_pal_enemy00);
            pal_primary_set(PAL_3_INDEX, RES_PAL_ENEMY01_SIZE, res_pal_enemy01);   
            pal_swap();                
            }
                        
        }

        //dma_queue_vram_transfer(font00_tiles, 100 * 16, 96 * 8 * 2, 2);
        //dma_queue_cram_transfer(font00_pal, 16, 16, 2);
        //dma_queue_vsram_transfer(data, 0, 2, 2);

        vid_vsync_wait();
        pal_update();
        dma_queue_flush();

    }
}

#if 0
int main()
{
    smd_ints_enable();
    //dma_vram_fill(VID_PLANE_A_ADDR, VID_PLANE_TILES <<, 0xFF, 2);
    plane_clear(PLANE_A);
    plane_clear(PLANE_B);
    while (1)
    {
 
        vid_vsync_wait();
    }
}
#endif