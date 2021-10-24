#include <stdint.h>
#include "vram.h"
#include "mddev.h"
#include "../../res/res.h"
//#include "tracks.h"

int main()
{
    uint16_t text[41];
    uint16_t size;
 
    smd_ints_disable();
    
    tiles_load_fast(res_font_sys, VRAM_INDEX_FONT, RES_FONT_SYS_SIZE);
    tiles_load_fast(res_img_skel_tileset, 1, RES_IMG_SKEL_TILESET_SIZE);

    pal_primary_set(PAL_0_INDEX, RES_PAL_PLAYER_SIZE, res_pal_player);
    pal_primary_set(PAL_1_INDEX, RES_PAL_COLLECTIBLES_SIZE, res_pal_collectibles);            
    pal_primary_set(PAL_2_INDEX, RES_PAL_ENEMY00_SIZE, res_pal_enemy00);
    pal_primary_set(PAL_3_INDEX, RES_PAL_ENEMY01_SIZE, res_pal_enemy01);   

    text_font_set(VRAM_INDEX_FONT);
    text_pal_set(PAL_1);
    size = text_render("SOUND TEST 00", text);
    plane_hline_draw(PLANE_A, text, 2, 2, size, false);

    z80_bus_request();
        //sound_sfx_set(64, door, RES_SFX_DOOR_SIZE);
        //sound_sfx_set(65, spit, RES_SFX_SPIT_SIZE);

        //sound_sfx_set(64, res_sfx_door, RES_SFX_DOOR_SIZE);
        //sound_sfx_set(65, res_sfx_spit, RES_SFX_SPIT_SIZE);
        sound_sfx_set(64, sfx_snd_death14, SFX_SND_DEATH14_SIZE);
        sound_sfx_set(65, sfx_snd_death14b, SFX_SND_DEATH14B_SIZE);
    z80_bus_release();

    smd_ints_enable();
    vid_display_enable();
    kdebug_alert("Iniciando cuerpo principal");
    while (1)
    {
        pad_update();

        /* Check press button  */
        if (pad_btn_pressed(PAD_1, PAD_BTN_A))
        {
            kdebug_alert("Boton A: Start sound 64");
            size = text_render("SOUND 1 PLAYING", text);
            plane_hline_draw(PLANE_A, text, 2, 4, size, false);                
            sound_sfx_play_auto(64, 15);
        }
        /* Check press button  */        
        if (pad_btn_pressed(PAD_1, PAD_BTN_B))
        {
            kdebug_alert("Boton B: Start sound 65");
            size = text_render("SOUND 2 PLAYING", text);
            plane_hline_draw(PLANE_A, text, 2, 4, size, false);                
            sound_sfx_play_auto(65, 15);
        }
        if (pad_btn_pressed(PAD_1, PAD_BTN_C))
        {
            if (sound_sfx_is_muted())
            {
                kdebug_alert("Boton C: Unmute sfx");
                size = text_render("UNMUTE SOUND", text);
                plane_hline_draw(PLANE_A, text, 2, 6, size, false);                
                sound_sfx_unmute();
            }
            else
            {
                kdebug_alert("Boton C: Mute sfx");
                size = text_render("MUTE SOUND  ", text);
                plane_hline_draw(PLANE_A, text, 2, 6, size, false);                
                sound_sfx_mute();
            }
            //pal_swap();                
        }

        //dma_queue_vram_transfer(font00_tiles, 100 * 16, 96 * 8 * 2, 2);
        //dma_queue_cram_transfer(font00_pal, 16, 16, 2);
        //dma_queue_vsram_transfer(data, 0, 2, 2);

        vid_vsync_wait();
        //sound_update();
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