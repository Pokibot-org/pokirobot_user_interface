import os, shutil

move_table = [
    ('./cubemx/f4693/Core/Src/main.c', './src/main.c'),
    ('./cubemx/f4693/Core/Src/stm32f4xx_hal_msp.c', './src/stm32f4xx_hal_msp.c'),
    ('./cubemx/f4693/Core/Src/stm32f4xx_hal_timebase_tim.c', './src/stm32f4xx_hal_timebase_tim.c'),
    ('./cubemx/f4693/Core/Src/stm32f4xx_it.c', './src/stm32f4xx_it.c'),

    ('./cubemx/f4693/Core/Inc/main.h', './include/main.h'),
    ('./cubemx/f4693/Core/Inc/stm32f4xx_hal_conf.h', './include/stm32f4xx_hal_conf.h'),
    ('./cubemx/f4693/Core/Inc/stm32f4xx_it.h', './include/stm32f4xx_it.h')
]

for (src, dest) in move_table:
    if os.path.exists(dest):
        os.remove(dest)
    shutil.copy(src, dest)