This example shows a line that keeps spinning on top of a gradient background image.

It decodes a PNG image resource into a frame buffer with vm_graphic_draw_image_memory, and draws a line with vm_graphic_draw_line, then start a timer with vm_timer_create_precise for animation. The timer runs every 1/10 secs, resulting in a 10 FPS animation. For each frame in the animation, vm_graphic_linear_transform is used to rotate the line, and then vm_graphic_blt_frame is called to composite the rotated line with the background image, and displays the result.

Modify the macro SCREEN_WIDTH and SCREEN_HEIGHT to fit your display device.
