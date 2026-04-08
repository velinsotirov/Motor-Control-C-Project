const float Ts_encoder = 0.001f;
const float Ts_controller = 0.01f;
const unsigned int controller_run_ticks = (int) (Ts_controller / Ts_encoder);
const float encoder_LP = 0.01f;
const unsigned int init_ticks = 100u;
const float max_current = 10.0f;
const float max_speed = 100.0f;

//char testchar = 0u;
//int testint = 0;
//short int testshort = 0;