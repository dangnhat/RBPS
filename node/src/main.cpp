/**
 * @file        main.cpp
 * @brief       Main program for nodes of Remote blood pressure and heart rate
 * 				monitoring system.
 *
 * @author      DangNhat Pham-Huu <51002279@hcmut.edu.vn>
 */

#include <cstdio>
#include <cstring>
#include "coocox.h"
#include "rbps_glb.h"
#include "znp_task.h"
#include "bat_inf.h"
#include "bpm_inf.h"


/*---------------------------------- Define  ---------------------------------*/
#define enable_znp
//#define fake_measure
#define Countdown_Total		180					/* Define max waiting time ( 5 minutes ) */

#define Controller_STK_SIZE		2048	 		/*!< Define 'Controller' stack size.				  */

#define	 Controller_PRI 		2		   	/*!< Priority of 'Controller' task.	  */

/* define state in Controller task */
enum Controller_State
{
   Startup,
   Enter_id,
   Checking_id,
   Information_retrieve,
   Idle,
   Warning_for_start_measure,
   hold_measurement,
   Countdown,
   Start_measure,
   Measuring,
   Display_result,
   Report,
   Display_predict,
   Send_cmd_update_node_id,
   Send_cmd_detail_info_id,
   Send_cmd_node_report_id,
   Key_process,
   Update_bat
};

/*---------------------------------- Variable Define -------------------------*/
OS_STK  Controller_Stk[Controller_STK_SIZE];	 	/*!< Stack of 'Controller' task.	  */

OS_STK  OneS_set_flag_Stk[Controller_STK_SIZE];	 	/*!< Stack of 'Controller' task.	  */

OS_STK  test_gui_Stk[Controller_STK_SIZE];	 	/*!< Stack of 'Controller' task.	  */

OS_FlagID	time_coundown_1s_flag; 	  /*!< Flag id,related to 'time_display' task.*/

void OneS_set_flag(void *pdata);
void Controller(void *pdata);
void gui_example(void *pdata);
void test_gui(void *pdata);

static uint16_t buffer_to_uint16(uint8_t *buffer);
static uint32_t buffer_to_uint32(uint8_t *buffer);
static void uint16_to_buffer(uint16_t data, uint8_t* buffer);
static void uint32_to_buffer(uint32_t data, uint8_t* buffer);
static float buffer_to_foat(uint8_t *buffer);
static void float_to_buffer(float data, uint8_t *buffer);

int main (void) {
	/* CoOS init */
	CoInitOS();


	/* System initialization */
	MB1_system_init();
	rbps_init();

	/* Create task and start OS */
	CoCreateTask(znp_task_func, 0, 0, &znp_task_stack[znp_task_stack_size-1], znp_task_stack_size);

	CoCreateTask(OneS_set_flag,0, 0, &OneS_set_flag_Stk[Controller_STK_SIZE - 1],Controller_STK_SIZE);
	CoCreateTask(Controller,0, 0, &Controller_Stk[Controller_STK_SIZE - 1],Controller_STK_SIZE);
//	CoCreateTask(test_gui,0, 0, &test_gui_Stk[Controller_STK_SIZE - 1],Controller_STK_SIZE);
	CoStartOS();

	while (1) {
		;
	}

	return 0;
}


/**
 *******************************************************************************
 * @brief		Controller task
 * @param[in] 	pdata	A pointer to parameter passed to task.
 * @param[out] 	None
 * @retval		None
 *
 * @details	    This task is called to start running system.
 *******************************************************************************
 */
void Controller(void *pdata)
{
	Controller_State Prev_state, Curr_state, Next_state, temp_state;
	uint16_t time_counter;
	bool Key_measure_cmd = false;
	bool Continous_flag;

	StatusType get_flag_result;

	keypad a_keypad;
	rbpm_gui a_gui;
	bat_inf	a_bat;
	bpm_inf a_bpm;

	bool longkey;
	int8_t key;
	uint32_t patient_id = 0;
	uint32_t temp;

	uint8_t data_buff[50];
	uint8_t cmd_buff[2];
	uint8_t patient_buff[50];
	void *mesg_p;
	StatusType mesg_status;

	MB1_ISRs.subISR_assign(ISRMgr_ns::ISRMgr_TIM6, keypad_timerISR);



	Prev_state = Startup;
	Curr_state = Startup;
	Next_state = Startup;
	temp_state = Startup;

	while(1)
	{
	   switch(Curr_state)
	   {
	   Next_state = Curr_state;
	   case Startup:
		   // Startup State
		   {
			   	a_gui.start_print_default_screen(); // test
#ifdef enable_znp
			   	{
			   		/* Update to coordinator with patient id = 0 */
					/* move data to main2znp_cir_queue */

			   		uint16_to_buffer(rbps_ns::update_node_id,cmd_buff);

			   		rbps_ns::main2znp_cir_queue.add_data((uint8_t) rbps_ns::update_node_length);

			   		rbps_ns::main2znp_cir_queue.add_data(cmd_buff,2);

			   		uint32_to_buffer((uint32_t)rbps_ns::node_id,data_buff); // node_id
			   		rbps_ns::main2znp_cir_queue.add_data(data_buff,4);

			   		uint32_to_buffer((uint32_t)0,data_buff); // patient id =0
			   		rbps_ns::main2znp_cir_queue.add_data(data_buff,4);

					CoPostQueueMail(rbps_ns::main2znp_queue_id, (void *)&rbps_ns::message_pending);
			   	}
#endif
			   	Next_state = Enter_id;

 				Prev_state = Startup;
				Curr_state = Next_state;
		   }
		   break;
	   case Enter_id:
		   // Enter_id State
		   {	Next_state = Enter_id;
		   	   Continous_flag = true;

		   /* if This state not looping, then refresh LCD */
	   	   if((Prev_state != Curr_state) && (Prev_state !=Update_bat)&& (Prev_state !=Countdown))
		   	   {
		   		a_gui.clear();
		   	    a_gui.start_print_default_screen();

		   	    patient_id = 0; // reset patient id
		   	   }

		   	    key = a_keypad.get_key(longkey);
				if ((key != keypad_ns::nokey) && (key < 10)) {
					patient_id = patient_id * 10 + key;
					a_gui.start_print_patient_id(patient_id);
				}
				else if ((key == 10) && (patient_id != 0)) {

					Next_state = Send_cmd_update_node_id;
					Continous_flag = false;
				}
				else if (key == 11) {
					a_gui.start_clear_patient_id();
					patient_id = 0;
				}

				if(Continous_flag)
				   {
					   Next_state = Update_bat;
				   }

				Prev_state = Enter_id;
				Curr_state = Next_state;

		   }
		   break;
	   case Send_cmd_update_node_id:
		   // Send_cmd_update_node_id State
		   {	Next_state = Send_cmd_update_node_id;


#ifdef enable_znp
			   	{
			   		/* Update to coordinator with patient id = 0 */
					/* move data to main2znp_cir_queue */

					rbps_ns::main2znp_cir_queue.add_data((uint8_t) rbps_ns::update_node_length);

					uint16_to_buffer(rbps_ns::update_node_id,(uint8_t*)cmd_buff);
			   		rbps_ns::main2znp_cir_queue.add_data((uint8_t*)cmd_buff,2);

			   		uint32_to_buffer((uint32_t)rbps_ns::node_id,(uint8_t*)data_buff); // node_id
			   		rbps_ns::main2znp_cir_queue.add_data((uint8_t*)data_buff,4);

			   		uint32_to_buffer((uint32_t)patient_id,(uint8_t*)data_buff); // patient id
			   		rbps_ns::main2znp_cir_queue.add_data((uint8_t*)data_buff,4);

					CoPostQueueMail(rbps_ns::main2znp_queue_id, (void *)&rbps_ns::message_pending);

					Next_state = Checking_id;
			   	}

#else
			   	Next_state = Checking_id;
#endif

				Prev_state = Send_cmd_update_node_id;
				Curr_state = Next_state;
		   }
		   break;
	   case Checking_id:
		   // Checking_id State
		   {
			   Next_state = Checking_id;
			   Continous_flag = true;

			   /* if This state not looping, then refresh LCD */
			   	   if((Prev_state != Curr_state) && (Prev_state !=Update_bat))
			   	   {
			   		a_gui.clear();
					a_gui.start_print_checking();
			   	   }

#ifdef enable_znp
			   	{
			   		mesg_p = CoAcceptQueueMail(rbps_ns::znp2main_queue_id, &mesg_status);
					if ((mesg_status == E_OK) && (*(uint8_t*)mesg_p == rbps_ns::message_pending))
					{
						// receive message
						rbps_ns::znp2main_cir_queue.get_data((uint8_t*)data_buff,4);
						temp = (uint16_t) data_buff[2];
						temp = ( temp <<8 )| data_buff[1];

//						 a_gui.start_print_patient_id(data_buff[1]);
//						 CoTimeDelay(0,0,2,0);
//						 a_gui.start_print_patient_id(data_buff[2]);
//						 CoTimeDelay(0,0,2,0);
//						a_gui.start_print_patient_id(1991);
//						CoTimeDelay(0,0,2,0);
//						 a_gui.start_print_patient_id(temp);
//						 CoTimeDelay(0,0,2,0);
						if(temp == rbps_ns::update_node_rep_id )
						{
							if(data_buff[3] == rbps_ns::status_true )
							{
								// patient is correct
								Next_state = Send_cmd_detail_info_id;
								Continous_flag = false;
							}
							else
							{
								// patient id incorrect
								a_gui.start_print_incorrect();
								CoTimeDelay(0,0,1,0);
								Next_state = Enter_id;
								Continous_flag = false;
							}
						}
					}

					 key = a_keypad.get_key(longkey);
									if (key == 11) {
										// cancel checking, return to startup state.
										Next_state = Startup;
										Continous_flag = false;
									}

			   	}
#else
			   	Next_state = Send_cmd_detail_info_id;
			   	Continous_flag = false;
#endif

			   	if(Continous_flag)
				   {
					   Next_state = Update_bat;
				   }
			   Prev_state = Checking_id;

			   Curr_state = Next_state;
		   }
		   break;
	   case Send_cmd_detail_info_id:
		   // Send_cmd_detail_info_id State
		   {	Next_state = Send_cmd_detail_info_id;

#ifdef enable_znp
			   	{
					rbps_ns::main2znp_cir_queue.add_data((uint8_t) rbps_ns::detail_info_length);

					uint16_to_buffer(rbps_ns::detail_info_id,(uint8_t*)cmd_buff);
					rbps_ns::main2znp_cir_queue.add_data((uint8_t*)cmd_buff,2);


					uint32_to_buffer((uint32_t)patient_id,(uint8_t*)data_buff); // patient id
					rbps_ns::main2znp_cir_queue.add_data((uint8_t*)data_buff,4);

					CoPostQueueMail(rbps_ns::main2znp_queue_id, (void *)&rbps_ns::message_pending);

					Next_state = Information_retrieve;
			   	}
#else
			   	Next_state = Information_retrieve;
#endif
				Prev_state = Send_cmd_detail_info_id;
				Curr_state = Next_state;
		   }
		   break;
	   case Information_retrieve:
		   //
		   { // send and receive information form coordinator
			   // TODO: implement information state.
			   Next_state = Information_retrieve;

//			   a_gui.welcome_print_default_screen((int8_t *)"Information_retrieve State");
//			   CoTimeDelay(0,0,1,0);
//
//			   key = a_keypad.get_key(longkey);
//
//			   if (key == 10) {
//
//					Next_state = Idle; // information is correct.
//				}
//				else if (key == 11) {
//					// clean id
//					patient_id = 0;
//					Next_state = Startup; // Information is incorrect, user want to re-enter it.
//				}


#ifdef enable_znp
			   	{
					mesg_p = CoAcceptQueueMail(rbps_ns::znp2main_queue_id, &mesg_status);
					if ((mesg_status == E_OK) && (*(uint8_t*)mesg_p == rbps_ns::message_pending))
					{
						// receive message
						rbps_ns::znp2main_cir_queue.get_data((uint8_t*)data_buff,7);
						temp = (uint16_t) data_buff[2];
						temp = ( temp <<8 )| data_buff[1];

//						a_gui.start_print_patient_id(1992);
//						CoTimeDelay(0,0,2,0);
//						 a_gui.start_print_patient_id(temp);
//						 CoTimeDelay(0,0,2,0);
						if(temp == rbps_ns::detail_info_rep_id )
						{
							// get name.
							rbps_ns::znp2main_cir_queue.get_data((uint8_t*)patient_buff,20);
//							   a_gui.welcome_print_default_screen((int8_t *)patient_buff);
//							   CoTimeDelay(0,0,1,0);
							Next_state = Idle;

						}
					}

					 key = a_keypad.get_key(longkey);
									if (key == 11) {
										// cancel checking, return to startup state.
										Next_state = Startup;
									}
			   	}
#else
			   	Next_state = Idle;
#endif

			   Prev_state = Information_retrieve;
			   Curr_state = Next_state;
		   }
		   break;
	   case Idle:
		   //
		   {
			   // Todo: implement Idle.

			   Key_measure_cmd = false;
			   Continous_flag = true;
			   time_counter = Countdown_Total;

			   /* if This state not looping, then refresh LCD */
		   	   if((Prev_state != Curr_state) && (Prev_state !=Update_bat))
			   	   {
			   		a_gui.clear();
			   		a_gui.welcome_print_default_screen((int8_t *)patient_buff);
			   	   }

			   // check for command form coordinator
			   	   // Check key pressed
			   	   	   // Update battery state
#ifdef enable_znp
			   	{
				   if(Continous_flag)
				   {
					   // Todo: implement Idle at check for command.
					   mesg_p = CoAcceptQueueMail(rbps_ns::znp2main_queue_id, &mesg_status);
					   if ((mesg_status == E_OK) && (*(uint8_t*)mesg_p == rbps_ns::message_pending))
						{
							// receive message
							rbps_ns::znp2main_cir_queue.get_data((uint8_t*)data_buff,3);

							temp = (uint16_t) data_buff[2];
							temp = ( temp <<8 )| data_buff[1];

//							a_gui.start_print_patient_id(1993);
//							CoTimeDelay(0,0,2,0);
//							 a_gui.start_print_patient_id(temp);
//							 CoTimeDelay(0,0,2,0);

							if(temp == rbps_ns::measure_node_id )
							{
								// Measure cmd
								Next_state = Warning_for_start_measure;
								Continous_flag = false;

							}
							else if(temp  == rbps_ns::prediction_rep_id )
							{
								// Measure cmd
								Next_state = Display_predict;
								Continous_flag = false;

							}
						}
				   }
			   	}
#endif
			   if(Continous_flag)
			   {
				   key = a_keypad.get_key(longkey);

				   			   if (key == 1) {

				   					Next_state = Start_measure; // debug
				   					Key_measure_cmd = true;
				   					Continous_flag = false;
				   				}

			   }
			   if(Continous_flag)
			   {

				   Next_state = Update_bat;
			   }

			   Prev_state = Idle;
			   Curr_state = Next_state;

		   }
		   break;
	   case Warning_for_start_measure:
		   //
		   {
			   // TODO: implement Warning_for_start_measure
			   // This state handle start measuring command from coordinator.
			   // Display message and count down to start measuring.
			   // If user press key 10 start measuring immediate.

			   Continous_flag = true;
// get all remain data in znp2main queue
			   rbps_ns::znp2main_cir_queue.get_data((uint8_t*)data_buff,4);

			   /* if This state not looping, then refresh LCD */
		   	   if((Prev_state != Curr_state) && (Prev_state !=Update_bat)&&(Prev_state !=Countdown)
		   			&&(Prev_state !=hold_measurement))
			   	   {

					a_gui.remote_print_screen(time_counter);
			   	   }

		   	if(Prev_state == Countdown)
		   	{
		   		a_gui.remote_print_measure_time(time_counter);
		   	}
//			   	a_gui.start_print_patient_id(time_counter);

			   if(Continous_flag)
			   {
				   key = a_keypad.get_key(longkey);

				   			   if (key == 10) {

				   					Next_state = Start_measure; //
				   					Continous_flag = false;
				   				}
				   			   if (key == 11)
				   			   {	// User expand the waiting time.
				   				   Next_state = hold_measurement; //
				   				   time_counter = time_counter + Countdown_Total;
				   				   if (time_counter > 1000)
				   				   {
				   					   time_counter = 1000;
				   				   }
				   				   Continous_flag = false;
				   			   }

			   }

			   if(Continous_flag)
			   {
				   if(time_counter <= 0)
				   {	// timeout
						Next_state = Start_measure; //
						Continous_flag = false;
				   }
			   }
			   if(Continous_flag)
			   {
				   switch(Prev_state)
				   {
				   case Update_bat:
					   {
						   Next_state = Countdown;
					   }
					   break;
				   case Countdown:
					   {
						   Next_state = Update_bat;
					   }
					   break;
				   default:
					   {
						   Next_state = Update_bat;
					   }
				   }
			   }

			   Prev_state = Warning_for_start_measure;
			   Curr_state = Next_state;
		   }
		   break;
	   case hold_measurement:
	   		   //
	   		   {
	   			   // Todo: implement hold_measurement
	   			   // This state send hold_measurement cmd to Coordinator
	   			   // then return previous state.

	   			   Next_state = Prev_state;
#ifdef enable_znp
			   	{
					rbps_ns::main2znp_cir_queue.add_data((uint8_t) rbps_ns::hold_measurement_length);

					uint16_to_buffer(rbps_ns::hold_measurement_id,(uint8_t*)cmd_buff); // cmd
					rbps_ns::main2znp_cir_queue.add_data((uint8_t*)cmd_buff,2);


					uint32_to_buffer((uint32_t)rbps_ns::node_id,(uint8_t*) data_buff); // node id
					rbps_ns::main2znp_cir_queue.add_data((uint8_t*) data_buff,4);

					data_buff[0] = 0x05; // hold 5 minutes.
					rbps_ns::main2znp_cir_queue.add_data((uint8_t*) data_buff,1);

					CoPostQueueMail(rbps_ns::main2znp_queue_id, (void *)&rbps_ns::message_pending);

			   	}
#endif


	   			   Prev_state = hold_measurement;
	   			   Curr_state = Next_state;
	   		   }
	   		   break;
	   case Countdown:
		   //
		   {
			   // Todo: implement Countdown.
			   Next_state = Prev_state;


			   if (time_counter > 0)

			   {
				   get_flag_result = CoAcceptSingleFlag (time_coundown_1s_flag);

				   if ( get_flag_result == E_OK)
				   {
					   time_counter--;
				   }
			   }

			   Prev_state = Countdown;
			   Curr_state = Next_state;
		   }
		   break;
	   case Start_measure:
		   // State
		   {
			   // Todo: implement Start_measure.
			   // init

		   	   if((Prev_state != Curr_state) && (Prev_state !=Update_bat))
		   	   {
		   		a_gui.clear();
		   		a_gui.puts((int8_t*)"Start_measure");
		   	   }



			   time_counter = Countdown_Total; // reset time_counter.
			   a_bpm.start_measure();

			   CoTimeDelay(0,0,7,0); // device need time to startup.

			   Next_state = Measuring;

			   Prev_state = Start_measure;
			   Curr_state = Next_state;
		   }
		   break;
	   case Measuring:
		   // State
		   {
			   // waiting for measure complete.
			   Continous_flag = true;

		   	   if((Prev_state != Curr_state) && (Prev_state !=Update_bat))
		   	   {
		   		a_gui.clear();
		   		a_gui.measuring_print_mesg();
		   	   }
		   	   CoTimeDelay(0,0,1,0); // test

			   if(a_bpm.measure_complete() == 1)
			   {
				   // measure complete.
				   Next_state = Display_result;
				   Continous_flag = false;
				   // turn off bpm device
				   CoTimeDelay(0,0,3,0);
				   a_bpm.press_menu();
			   }

			   if( Key_measure_cmd)
			   {
				   // measuring start by keypad then can stop by keypad
				   key = a_keypad.get_key(longkey);

				   			   if (key == 11) {

				   					Next_state = Idle; //
				   					Continous_flag = false;

				 				   // turn off bpm device
				 				   a_bpm.press_menu();
				   				}

			   }
			   if(Continous_flag)
			   {
				   Next_state = Update_bat;
			   }


			   Prev_state = Measuring;
			   Curr_state = Next_state;
		   }
		   break;
	   case Display_result:
		   // State
		   {
			   // Todo: implement Display_result.
			   Continous_flag = true;
			   Next_state = Curr_state;
			   // Display something.
		   	   if(Prev_state == Measuring)
		   	   {
		   		a_gui.clear();
				a_gui.puts((int8_t*)"Display_result");
				time_counter = 5;


//#ifndef fake_measure
//				{
					CoTimeDelay(0,0,1,0);
					a_bpm.enter_pcl_mode();
					a_bpm.get_measure_value();
					a_bpm.exit_pcl_mode();
//				}
//#endif

			   a_gui.direct_print_default_screen((short int) a_bpm.get_sys_value(),
					   (short int) a_bpm.get_dia_value(),
					   (short int) a_bpm.get_pulse_value(),
					   0,0,0,0,0,0,0,0,0);
		   	   }

			   if(Continous_flag)
			   {
				   key = a_keypad.get_key(longkey);

				   			   if ((key == 10)||(key == 11))
				   			   {

				   					Next_state = Report; //
				   					Continous_flag = false;
				   				}
			   }

			   if(Continous_flag)
			   {
				   if(time_counter <= 0)
				   {	// timeout
						Next_state = Report; //
						Continous_flag = false;
				   }
			   }
			   if(Continous_flag)
			   {
				   switch(Prev_state)
				   {
				   case Update_bat:
					   {
						   Next_state = Countdown;
					   }
					   break;
				   case Countdown:
					   {
						   Next_state = Update_bat;
					   }
					   break;
				   default:
					   {
						   Next_state = Update_bat;
					   }
				   }
			   }

			   Prev_state = Display_result;
			   Curr_state = Next_state;
		   }
		   break;
	   case Report:
		   // State
		   {
			   // Todo: implement Report.
			   // Send data to Coordinator.

				a_gui.clear();
				a_gui.puts((int8_t*)"Report");
				CoTimeDelay(0,0,1,0);

#ifdef enable_znp
			   	{
			   		rbps_ns::main2znp_cir_queue.add_data((uint8_t) rbps_ns::node_report_length);

			   		uint16_to_buffer(rbps_ns::node_report_id,(uint8_t*)cmd_buff); // command
			   		rbps_ns::main2znp_cir_queue.add_data((uint8_t*)cmd_buff,2);

			   		uint32_to_buffer((uint32_t)rbps_ns::node_id,(uint8_t*)data_buff); // node_id
			   		rbps_ns::main2znp_cir_queue.add_data((uint8_t*)data_buff,4);

			   		uint32_to_buffer((uint32_t)a_bpm.get_sys_value(),(uint8_t*)data_buff); // sys_value
			   		rbps_ns::main2znp_cir_queue.add_data((uint8_t*)data_buff,4);

			   		uint32_to_buffer((uint32_t)a_bpm.get_dia_value(),(uint8_t*)data_buff); // dia_value
			   		rbps_ns::main2znp_cir_queue.add_data((uint8_t*)data_buff,4);

			   		uint32_to_buffer((uint32_t)a_bpm.get_pulse_value(),(uint8_t*)data_buff); // pulse_value
			   		rbps_ns::main2znp_cir_queue.add_data((uint8_t*)data_buff,4);

			   		uint16_to_buffer((uint16_t) 0,(uint8_t*)data_buff); // time stamp
			   		rbps_ns::main2znp_cir_queue.add_data((uint8_t*)data_buff,2);
			   		rbps_ns::main2znp_cir_queue.add_data((uint8_t*)data_buff,2);
			   		rbps_ns::main2znp_cir_queue.add_data((uint8_t*)data_buff,2);

			   		if(Key_measure_cmd)
			   		{
			   			data_buff[0] = 1;
			   		}
			   		else
			   		{
			   			data_buff[0] = 0;
			   		}
			   		rbps_ns::main2znp_cir_queue.add_data((uint8_t*)data_buff,1); // need predict

					CoPostQueueMail(rbps_ns::main2znp_queue_id, (void *)&rbps_ns::message_pending);
			   	}
#endif
			   Next_state = Idle;

			   Prev_state = Report;
			   Curr_state = Next_state;
		   }
		   break;
	   case Display_predict:
		   //  State
		   {
			   // Todo: implement Display_predict.
			   Continous_flag = true;
			   // Display_predict and move line on screen.
			   // get remain data in znp2main queue
#ifdef enable_znp
			   {
				   if (Prev_state == Idle)
				   {
					   rbps_ns::znp2main_cir_queue.get_data((uint8_t*)data_buff,21);

					   a_gui.direct_print_default_screen(
							   (short int) a_bpm.get_sys_value(),
							   (short int) a_bpm.get_dia_value(),
							   (short int) a_bpm.get_pulse_value(),
							   (bool)data_buff[0],
							   buffer_to_uint32((uint8_t*)data_buff + 1),
							   buffer_to_uint32((uint8_t*)data_buff + 5),
							   buffer_to_uint32((uint8_t*)data_buff + 9),
							   buffer_to_uint32((uint8_t*)data_buff + 13),
							   (bool)data_buff[17],
							   (bool)data_buff[18],
							   (bool)data_buff[19],
							   (bool)data_buff[20]);
				   }
			   }
#endif
			   if(Continous_flag)
			   {
				   key = a_keypad.get_key(longkey);

				   			   if (key == 10) {

				   					Next_state = Idle; //
				   					Continous_flag = false;
				   				}
				   			   if (key == 1)
				   			   {	// Roll up.
				   				   a_gui.direct_scroll_up();
				   			   }
				   			   if (key == 2)
				   			   {	// Roll down.
				   				   a_gui.direct_scroll_down();
				   			   }

			   }

			   if(Continous_flag)
			   {
				   if(time_counter <= 0)
				   {	// timeout.
	   					Next_state = Idle; //
	   					Continous_flag = false;
				   }
			   }
			   if(Continous_flag)
			   {
				   switch(Prev_state)
				   {
				   case Update_bat:
					   {
						   Next_state = Countdown;
					   }
					   break;
				   case Countdown:
					   {
						   Next_state = Update_bat;
					   }
					   break;
				   default:
					   {
						   Next_state = Update_bat;
					   }
				   }


			   }

			   Prev_state = Display_predict;
			   Curr_state = Next_state;
		   }
		   break;
	   case Key_process:
		   // State
		   {
			   Next_state = Idle;
			   Prev_state = Key_process;
			   Curr_state = Next_state;
		   }
		   break;
	   case Update_bat:
		   // State Update battery indicator on LCD
		   // Go back to previous state update battery status.
		   {
			   Next_state = Prev_state;

			   a_gui.print_battery_status(a_bat.get_bat_percent());
			   Prev_state = Update_bat;
			   Curr_state = Next_state;
		   }
		   break;
	   default :
		   Curr_state = Startup;
	   }
	}

}
/**
 * @brief   set flag after 1 second for countdown.
 * //TODO: 			remember to start this task
 */

void OneS_set_flag(void *pdata)
{
	time_coundown_1s_flag = CoCreateFlag (1, 0);

	while(1)
	{
		CoTimeDelay(0,0,1,0);
		CoSetFlag(time_coundown_1s_flag);

	}
}
/**
 * @brief   test gui task.
 *
 */

void test_gui(void *pdata)
{
	rbpm_gui a_gui;
	uint8_t temp = 0;
	a_gui.direct_print_default_screen(0,0,0,0,0,0,0,0,0,0,0,0);
	CoTimeDelay(0,0,2,0);

	a_gui.puts((int8_t*)"test choi thoi ma");
	CoTimeDelay(0,0,2,0);
	while(1)
	{
		if( CoAcceptSingleFlag (time_coundown_1s_flag) == E_OK)
		{
			temp++;
			a_gui.start_print_patient_id(temp);
			CoTimeDelay(0,0,1,0);
			a_gui.clear();
			CoTimeDelay(0,0,1,0);
			a_gui.puts((int8_t*)"test choi thoi ma");
		}
	}
}

/**
 * @brief   gui example code //TODO: remove when it become useless.
 */
void gui_example(void *pdata) {
	keypad a_keypad;
	bool longkey;
	int8_t key;
	uint32_t patient_id = 0;

	MB1_ISRs.subISR_assign(ISRMgr_ns::ISRMgr_TIM6, keypad_timerISR);

	rbpm_gui a_gui;
	a_gui.start_print_default_screen();
	while (1) {
		key = a_keypad.get_key(longkey);

		if ((key != keypad_ns::nokey) && (key < 10)) {
			patient_id = patient_id * 10 + key;
			a_gui.start_print_patient_id(patient_id);
		}
		else if (key == 10) {
			break;
		}
		else if (key == 11) {
			a_gui.start_clear_patient_id();
			patient_id = 0;
		}
	}

	/* check patient id */
	a_gui.start_print_checking();
	CoTimeDelay(0, 0, 1, 0);

	if (patient_id != 1992 && patient_id != 13456) {
		a_gui.start_print_incorrect();
	}
	else {
		a_gui.welcome_print_default_screen((int8_t *)"Pham Huu Dang Nhat");

		while (1) {
			key = a_keypad.get_key(longkey);

			if (key == 1) {
				break;
			}
		}

		a_gui.measuring_print_mesg();
		CoTimeDelay(0, 0, 1, 0);

		a_gui.direct_print_default_screen(120, 78, 60, false, 123, 79, 67, 23, true, false, true, false);

		while (1) {
			key = a_keypad.get_key(longkey);

			if (key == 10) {
				if (longkey) {
					while(a_keypad.is_longkey_still_hold(key)){
						a_gui.direct_scroll_down();
						CoTimeDelay(0, 0, 0, 300);
					}
				}
				else {
					a_gui.direct_scroll_down();
				}
			}
			else if (key == 11) {
				if (longkey) {
					while(a_keypad.is_longkey_still_hold(key)){
						a_gui.direct_scroll_up();
						CoTimeDelay(0, 0, 0, 300);
					}
				}
				else {
					a_gui.direct_scroll_up();
				}
			}
		}
	}

	while(1) {
		printf("This is znp task\n");
		CoTimeDelay(0, 0, 1, 0);
	}
}

/*----------------------------------------------------------------------------*/
static uint16_t buffer_to_uint16(uint8_t *buffer) {
	uint16_t ret_val;

	ret_val = (*buffer) | (*(buffer + 1) << 8);

	return ret_val;
}

/*----------------------------------------------------------------------------*/
static uint32_t buffer_to_uint32(uint8_t *buffer) {
	uint32_t ret_val;

	ret_val = (*buffer) | (*(buffer + 1) << 8) | (*(buffer + 2) << 16) | (*(buffer + 3) << 24);

	return ret_val;
}

/*----------------------------------------------------------------------------*/
static void uint16_to_buffer(uint16_t data, uint8_t* buffer) {
	*buffer = (uint8_t)data;
	*(buffer+1) = (uint8_t)(data >> 8);
}

/*----------------------------------------------------------------------------*/
static void uint32_to_buffer(uint32_t data, uint8_t* buffer) {
	*buffer = (uint8_t)data;
	*(buffer+1) = (uint8_t)(data >> 8);
	*(buffer+2) = (uint8_t)(data >> 16);
	*(buffer+3) = (uint8_t)(data >> 24);
}

/*----------------------------------------------------------------------------*/
static float buffer_to_float(uint8_t *buffer) {
	float ret_val;
	uint16_t dec;
	uint16_t frac;

	dec = buffer_to_uint16(buffer);
	frac = buffer_to_uint16(buffer + 2);

	ret_val = (float)dec;
	ret_val += ((float)frac)/100;

	return ret_val;
}

/*----------------------------------------------------------------------------*/
static void float_to_buffer(float data, uint8_t *buffer) {
	uint16_t dec, frac;

	dec = (uint16_t) data;
	frac = ((uint16_t) (data * 100)) % 100;

	uint16_to_buffer(dec, buffer);
	uint16_to_buffer(frac, buffer+2);
}
