    .section .rodata

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    .global res_image_f
    .global res_image_f_size
res_image_f:
    .incbin "image.f.glsl"
    .byte 0
1:
res_image_f_size:
    .int 1b - res_image_f

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    .global res_image_v
    .global res_image_v_size
res_image_v:
    .incbin "image.v.glsl"
    .byte 0
1:
res_image_v_size:
    .int 1b - res_image_v
