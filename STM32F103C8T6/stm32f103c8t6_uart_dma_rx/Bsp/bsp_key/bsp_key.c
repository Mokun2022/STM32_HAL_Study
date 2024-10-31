#include "bsp_key.h"


uint8_t bsp_key_scanf(void)
{
	static char kp = 0;	
	
	if(1 !=GET_KEY_GPIO)
	{
		HAL_Delay(10);
		if((1 !=GET_KEY_GPIO) && (kp == 0))
		{
			kp = 1;
			return 1;
		}
	}
	else
		kp = 0;
	
	return 0;
}
