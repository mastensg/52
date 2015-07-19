    .section .rodata

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    .global res_dot_f
    .global res_dot_f_size
res_dot_f:
    .incbin "dot.f.glsl"
    .byte 0
1:
res_dot_f_size:
    .int 1b - res_dot_f

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    .global res_dot_v
    .global res_dot_v_size
res_dot_v:
    .incbin "dot.v.glsl"
    .byte 0
1:
res_dot_v_size:
    .int 1b - res_dot_v
