#ifndef HARDWARE_H_
#define HARDWARE_H_

/*
 PTA5/IRQ/TCLK/RESET  (1)       (20)  PTA0/PIA0/TPM1CH0/ADP0/ACMP+ 
  PTA4/ACMPO/BKGD/MS  (2)       (19)  PTA1/PIA1/TPM2CH0/ADP1/ACMP-    
                 VDD  (3)       (18)  PTA2/PIA2/SDA/ADP2
                 VSS  (4)       (17)  PTA3/PIA3/SCL/ADP3
      PTB7/SCL/EXTAL  (5)       (16)  PTB0/PIB0/RxD/ADP4
       PTB6/SDA/XTAL  (6)       (15)  PTB1/PIB1/TxD/ADP5
     PTB5/TPM1CH1/SS  (7)       (14)  PTB2/PIB2/SPSCK/ADP6
   PTB4/TPM2CH1/MISO  (8)       (13)  PTB3/PIB3/MOSI/ADP7
          PTC3/ADP11  (9)       (12)  PTC0/TPM1CH0/ADP8
          PTC2/ADP10  (10)      (11)  PTC1/TPM1CH1/ADP9
*/

//Puerto A
#define dir_ntc				PTADD_PTADD0
#define dir_color				PTADD_PTADD1
#define dir_motorCC				PTADD_PTADD2
#define dir_cooler				PTADD_PTADD3

//Puerto B
#define dir_teclado			PTBDD_PTBDD0
#define dir_com_tx 				PTBDD_PTBDD1
#define	dir_celda_carga  				PTBDD_PTBDD2
#define dir_FC1					PTBDD_PTBDD3
#define dir_FC2					PTBDD_PTBDD4
#define dir_lcd_data			PTBDD_PTBDD5
#define dir_lcd_hab				PTBDD_PTBDD6
#define dir_lcd_clk				PTBDD_PTBDD7

//Puerto C
#define dir_motorPP				PTCDD

//Puerto A
#define ntc				PTAD_PTAD0
#define color				PTAD_PTAD1
#define motorCC				PTAD_PTAD2
#define cooler				PTAD_PTAD3

//Puerto B
//#define teclado			PTBD_PTBD0
#define com_tx 				PTBD_PTBD1
#define celda_carga			PTBD_PTBD2
#define FC1 				PTBD_PTBD3
#define FC2					PTBD_PTBD4
#define lcd_data			PTBD_PTBD5
#define lcd_hab 			PTBD_PTBD6
#define lcd_clk 			PTBD_PTBD7

//Puerto C
#define motorPP				PTCD

#endif /* HARDWARE_H_ */
