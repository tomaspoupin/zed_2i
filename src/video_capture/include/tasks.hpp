#ifndef __VID_TASKS__
#define __VID_TASKS__

static void record_step(sl::Camera *camera, sl::RuntimeParameters& params)
{
    sl::SensorsData data;
    camera->grab(params);
}

#endif