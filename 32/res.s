    .section .rodata

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    .global res_text_f
    .global res_text_f_size
res_text_f:
    .incbin "text.f.glsl"
    .byte 0
1:
res_text_f_size:
    .int 1b - res_text_f

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    .global res_text_v
    .global res_text_v_size
res_text_v:
    .incbin "text.v.glsl"
    .byte 0
1:
res_text_v_size:
    .int 1b - res_text_v
