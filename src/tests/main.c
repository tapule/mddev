#include <stdint.h>
#include "vram.h"
#include "mddev.h"
#include "../../res/res.h"
//#include "tracks.h"

int main()
{
    uint16_t text[128];
    uint16_t size;
    uint16_t status = 0;
    uint16_t song = 0;
    uint16_t sfx = 0;


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
        sound_sfx_set(64, res_sfx_door, RES_SFX_DOOR_SIZE);
        sound_sfx_set(65, res_sfx_spit, RES_SFX_SPIT_SIZE);
        sound_sfx_set(66, sfx_snd_death14, SFX_SND_DEATH14_SIZE);
    z80_bus_release();

    sound_music_play(mus_credits);

    smd_ints_enable();
    vid_display_enable();
    kdebug_alert("Iniciando cuerpo principal");
    while (1)
    {
        pad_update();

        /* Check press button  */
        if (pad_btn_pressed(PAD_1, PAD_BTN_A))
        {
            switch (sfx)
            {
            case 0:
                ++sfx;
                kdebug_alert("Boton A: Start sound 0");
                size = text_render("SOUND 0 PLAYING", text);
                sound_sfx_play_auto(64, 15);                
                break;
            case 1:
                ++sfx;
                kdebug_alert("Boton A: Start sound 1");
                size = text_render("SOUND 1 PLAYING", text);
                sound_sfx_play_auto(65, 15);                
                break;
            case 2:
                sfx = 0;
                kdebug_alert("Boton A: Start sound 0");
                size = text_render("SOUND 2 PLAYING", text);
                sound_sfx_play_auto(66, 15);                
                break;
            }
            plane_hline_draw(PLANE_A, text, 2, 4, size, false);  
        }
        /* Check press button  */        
        if (pad_btn_pressed(PAD_1, PAD_BTN_B))
        {
            kdebug_alert("Boton B: Pause/Resume/Switch song");
            switch (status)
            {
            // Pause
            case 0:
                size = text_render("MUSIC PAUSE   ", text);            
                sound_music_pause();
                status = 1;
                break;
            // Resume
            case 1:
                size = text_render("MUSIC RESUME   ", text);            
                sound_music_resume();
                status = 0;
                break;
            // Start new song
            case 4:
                switch (song)
                {
                case 0:
                    size = text_render("MUSIC SONG 0", text);            
                    sound_music_play(mus_credits);
                    break;
                case 1:
                    size = text_render("MUSIC SONG 1", text);            
                    sound_music_play(mus_demo);
                    break;
                case 2:
                    size = text_render("MUSIC SONG 2", text);            
                    sound_music_play(mus_caves);
                    break;
                }
                status = 0;
                break;
            }
            plane_hline_draw(PLANE_A, text, 2, 6, size, false);      
        }
        if (pad_btn_pressed(PAD_1, PAD_BTN_C))
        {
            sound_music_stop();
            status = 4;
            ++song;
            if (song == 3)
            {
                song = 0;
            }
            size = text_render("MUSIC STOP  ", text);    
            plane_hline_draw(PLANE_A, text, 2, 6, size, false);      
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