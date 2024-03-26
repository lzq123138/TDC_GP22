/******************************************************************************* 
* Function Name  : main 
* Description    : Main program. 
* Input          : None 
* Output         : None 
* Return         : None 
*******************************************************************************/ 
void main(void) 
{    ENTR_CRT_SECTION();   /* Setup STM32 system (clock, PLL and Flash configuration) */   
     SystemInit();    
     EXT_CRT_SECTION();    // Choose your Slot (SPI1, SPI2)   
	   void* Bus_Type = SPI1;      /* controlled loop */   
	   while (Dummy_var!=11) 
			 // To control the loop, e.g. (Dummy_var!=7)    
		 {     if (Dummy_var==10) 
			           Dummy_var=0; // Infinite loop          
			     if(configured_true==FALSE)     
						 {         
							 configured_true = TRUE;       
							 SPIx_GPIOs_Init(Bus_Type);       
							 SPIx_Interface_Init(Bus_Type);              
							 Ext_Interrupt_Init();              
							 gp22_send_1byte(Bus_Type, Power_On_Reset);       
							 Dly100us((void*)5);              // 500 us wait for GP22         
							 // Writing to the configuration registers (CR)       
							 // CR0: DIV_CLKHS = 2, START_CLKHS = 1, CALIBRATE = 0, NO_CAL_AUTO = 1, MESSB2 = 0, NEG_STOP = NEGSTART = 0, ...       
							 gp22_wr_config_reg(Bus_Type, 0x80, 0x00241000);         
							 
							 // CR1: EN_FAST_INIT = 1, HITIN2 = 1, HITIN1 = 1, ...       
							 gp22_wr_config_reg(Bus_Type, 0x81, 0x19C90000);        
							 
							 // CR2: EN_INT = b111, RFEDGE1 = RFEDGE2 = 0, ...        
							 gp22_wr_config_reg(Bus_Type, 0x82, 0xE0000000);        
							 
							 // CR3: ...        
							 gp22_wr_config_reg(Bus_Type, 0x83, 0x00000000);       
							 
							 // CR4: ...       
							 gp22_wr_config_reg(Bus_Type, 0x84, 0x20000000);        
							 
							 // CR5: CON_FIRE = b000, EN_STARTNOISE = 1, ...       
							 gp22_wr_config_reg(Bus_Type, 0x85, 0x10000000);        
							 
							 // CR6: QUAD_RES = 0, ...       
							 gp22_wr_config_reg(Bus_Type, 0x86, 0x00000000);      
							 }      
						 // .........................................................................     
						// ........................Calibrate High Speed Clock.......................     
					 // ...................Laser Rangefinder Measurement CYCLE...................     
							 // .........................Caluculate Result Values........................          
							 N_Measure_Cycles = 50000;          
							 diff_Cal2_Cal1_old = diff_Cal2_Cal1_new;  
							 if((Dummy_var==0) | (Dummy_var==10)) 
								 {     
									 //--------------------------------------------------------------------------  
									 // Start Calibrate High Speed Clock Cycle     
									 gp22_send_1byte(Bus_Type, Init);     
									 gp22_send_1byte(Bus_Type, Start_Cal_Resonator);      
									 // Wait for INT Slot_x     
									 if (Bus_Type==SPI1) 
										 while (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_4)==1);      
									 if (Bus_Type==SPI2) 
										 while (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_11)==1);             
									 //Calculate Correction factor     
									 //The time interval to be measured is set by ANZ_PER_CALRES     
									 //which defines the number of periods of the 32.768 kHz clock:     
									 //2 periods = 61.03515625 µs     CLKHS_freq_corr_fact = 61.03515625/       
									 gp22_read_n_bytes(Bus_Type, 4, 0xB0, 0x00, 16) * CLKHS_freq;         
									 printf("\n Correction factor for clock = %1.4f\n", CLKHS_freq_corr_fact);              
									 CLKHS_freq_cal = CLKHS_freq * CLKHS_freq_corr_fact; 
									 // Calibrated Clock frequency 
									 }
									 //--------------------------------------------------------------------------    
									 // Start Seperate Calibration Measurement Cycle         
									 gp22_send_1byte(Bus_Type, Init);          
									 gp22_send_1byte(Bus_Type, Start_Cal_TDC); 
									 // update calibration data      
									 // Note:     
									 // The calibration data are not addressed directly after the calibration     
									 // measurement but after the next regular measurement, before the next INIT.          
									 // Wait for INT Slot_x     
									 if (Bus_Type==SPI1) 
										 while (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_4)==1);      
									 if (Bus_Type==SPI2) 
										 while (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_11)==1);      
									 gp22_wr_config_reg(Bus_Type, 0x81, 0x19490000);      
									 //--------------------------------------------------------------------------     
									 // 1st ToF Measurement plus calibratio data readout     
									 gp22_send_1byte(Bus_Type, Init);      
									 //Trigger pulse laser     
									 //    SetPortHigh;        
									 GPIO_WriteBit(GPIOD, GPIO_Pin_8, Bit_SET);// Output HIGH     
									 //    SetPortLow;     
									 GPIO_WriteBit(GPIOD, GPIO_Pin_8, Bit_RESET);   // Output LOW          
									 // Wait for INT Slot_x     
									 if (Bus_Type==SPI1) 
										 while (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_4)==1);      
									 if (Bus_Type==SPI2) 
										 while (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_11)==1);      
									 // First regular measurement (to readout calibration data)     
									 RAW_Result = gp22_read_n_bytes(Bus_Type,4,0xB0,0x00,16); //    
									 printf("\n 1. Measured RAW Value = %.0f \n",RAW_Result); // RAW value      
									 // Check Status Register, next free result register //    
									 printf("Stat_Reg = 0x%04X \n",gp22_read_status_bytes(Bus_Type));       
									 // readout the new calibration data from result register adr 0x01     
									 gp22_wr_config_reg(Bus_Type, 0x81, 0x67490000);     
									 diff_Cal2_Cal1_new = gp22_read_n_bytes(Bus_Type,4,0xB0,0x01,16);          
									 //--------------------------------------------------------------------------     
									 // Caluculate the real time after the hole first cycle loop     
									 while (diff_Cal2_Cal1_old != 0)      
										 {         
											 avg_diff_Cal2_Cal1 = (diff_Cal2_Cal1_new+diff_Cal2_Cal1_old) / 2;  
											 //        
											 printf("\n OLD Cal2-Cal1 RAW Value = %.0f \n",diff_Cal2_Cal1_old); 
											 //        
											 printf("\n NEW Cal2-Cal1 RAW Value = %.0f \n",diff_Cal2_Cal1_new);          
											 average_RAW_Result /= N_Measure_Cycles;              // Used Formulas:         
											 // ---------------------------------------------------         
											 //                 T_ref         
											 // Time_Value = ----------- * measured_RAW_Value         
											 //               Cal2-Cal1         
											 // ---------------------------------------------------         
											 //                   velocity_of_light         
											 // Distance_Value = ------------------- * Time_Value         
											 //                           2         
											 // ---------------------------------------------------                 
											 // For this Source Code would be a Reference Clock used with 1 MHz                  
											 Time_Result = (average_RAW_Result/avg_diff_Cal2_Cal1) * 1000;//time [ns]          
											 Distance_Result = Time_Result / 6.671281904; //distance [m]                 
											 printf("\n Time Measure Result (ToF) = %.3f ns\n",Time_Result);         
											 printf(" corresponds to %.3f m of Distance\n",Distance_Result);         
											 printf(" to reflected point after %u Measurements\n",N_Measure_Cycles);          
											 diff_Cal2_Cal1_old = 0;     
											 }      
										 //--------------------------------------------------------------------------     // 
											 if more than one measure cycle     average_RAW_Result = RAW_Result; 
											 // set first value of average_RAW_Result          
											 gp22_wr_config_reg(Bus_Type, 0x81, 0x19C90000); // EN_FAST_INIT = 1     
											 gp22_send_1byte(Bus_Type, Init);          //Trigger pulse laser     
											 //    SetPortHigh;        
											 GPIO_WriteBit(GPIOD, GPIO_Pin_8, Bit_SET);   // Output HIGH     
											 //    SetPortLow;     
											 GPIO_WriteBit(GPIOD, GPIO_Pin_8, Bit_RESET); // Output LOW          
											 //--------------------------------------------------------------------------     
											 // n'th ToF Measurement     
											 for (int i=2; i<=N_Measure_Cycles;i++)     
											 {         // Wait for INT Slot_x        
												 if (Bus_Type==SPI1) while (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_4)==1);          
												 if (Bus_Type==SPI2) while (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_11)==1);          
												 RAW_Result = gp22_read_n_bytes(Bus_Type,4,0xB0,0x00,16); //        
												 printf(" %u. Measure RAW Value = %.0f \n",i,RAW_Result); // RAW value         
												 average_RAW_Result += RAW_Result;                  //Trigger pulse laser         
												 //    SetPortHigh;            
												 GPIO_WriteBit(GPIOD, GPIO_Pin_8, Bit_SET);   // Output HIGH         
												 //    SetPortLow;         
												 GPIO_WriteBit(GPIOD, GPIO_Pin_8, Bit_RESET); // Output LOW      
												 }          //--------------------------------------------------------------------------      
											 printf("\nNEW CYCLE...\n");       Dummy_var++; // To Control the loop        
												 }
												 // End while Dummy_var 																								 
} 
//End main  
												 
												 
												 
												 