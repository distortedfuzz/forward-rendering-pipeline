## CENG477 HW2

## Forward Rendering Pipeline

Grade: 95.0/100.0

This is an implementation of the full forward rendering pipeline upto rasterization with C++ featuring backface culling and clipping.

There are solid and wireframe modes to render objects.

The clipping algorithm has problems, does not work on some lines.

Everything except reading the files, the basic matrix and vector classes is my work.

>make rasterizer
>
>./rasterizer <input_file_name>


Sample outputs:


![filled_box_3](https://github.com/distortedfuzz/forward-rendering-pipeline/assets/90470997/1badb5a6-8072-4c70-8800-dc52d4886a95)


![horse_and_mug_perspective_1](https://github.com/distortedfuzz/forward-rendering-pipeline/assets/90470997/226d72dc-b60f-44ee-9367-4b2ecc7cc594)


![horse_and_mug_perspective_2](https://github.com/distortedfuzz/forward-rendering-pipeline/assets/90470997/1f374bdf-9748-4aad-bf9d-0b224c5bb2c1)


![empty_box_2](https://github.com/distortedfuzz/forward-rendering-pipeline/assets/90470997/60a85999-bd29-4019-8b9a-7ca0f6c5db32)


![flag_turkey_alt_2](https://github.com/distortedfuzz/forward-rendering-pipeline/assets/90470997/6f56cb2b-ee6f-4888-a0be-3ebbf76e96e9)
