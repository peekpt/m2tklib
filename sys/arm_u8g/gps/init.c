/*

  init.c 
  
  Init procedure of the GPS tracker device.
  
  m2tklib - Mini Interative Interface Toolkit Library
  u8glib - Universal 8bit Graphics Library
  
  Copyright (C) 2013  olikraus@gmail.com

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
  
*/


#include "LPC11xx.h"
#include "u8g_arm.h"
#include "uart.h"
#include "init.h"


/*========================================================================*/
/* variables of the gps tracker device */

struct gps_tracker_variables_struct gps_tracker_variables;


/*========================================================================*/
/* SystemInit & SysTick Interrupt */

#define SYS_TICK_PERIOD_IN_MS 10

void SystemInit()
{    
  init_system_clock();		/* SystemCoreClock will be set here */
  
  /* SysTick is defined in core_cm0.h */
  SysTick->LOAD = (SystemCoreClock/1000UL*(unsigned long)SYS_TICK_PERIOD_IN_MS) - 1;
  SysTick->VAL = 0;
  SysTick->CTRL = 7;   /* enable, generate interrupt (SysTick_Handler), do not divide by 2 */
}

void __attribute__ ((interrupt, used)) SysTick_Handler(void)
{
  gps_tracker_variables.cnt_10ms++;
  if ( gps_tracker_variables.cnt_10ms >= 100 )
  {
    gps_tracker_variables.cnt_10ms = 0;
    gps_tracker_variables.sec_cnt_raw++;    
  }
}

/*========================================================================*/
/* setup gps device */

pq_t pq;

void gps_rx(uint8_t value)
{
  gps_tracker_variables.uart_byte_cnt_raw++;
  pq_AddChar(&pq, (uint8_t)value);
}

#define GPS_HALF_MAP_CNT 9
const char *gps_half_map_str[GPS_HALF_MAP_CNT] = { "5m", "10m", "20m", "50m", "100m", "200m", "500m", "1km", "2km" };
uint16_t gps_half_map_len[GPS_HALF_MAP_CNT] = { 5, 10, 20, 50, 100, 200, 500, 1000, 2000 };


void gps_set_half_map_size_by_index(uint8_t idx)
{
  if ( idx >= GPS_HALF_MAP_CNT )
    idx = GPS_HALF_MAP_CNT-1;
  gps_tracker_variables.half_map_size_index = idx;
  gps_tracker_variables.half_map_size = gps_half_map_len[idx];
}

void gps_inc_half_map_size(void)
{
  if ( gps_tracker_variables.half_map_size_index < GPS_HALF_MAP_CNT-1 )
    gps_tracker_variables.half_map_size_index++;
    
  /*
  gps_tracker_variables.half_map_size_index++;
  if ( gps_tracker_variables.half_map_size_index >= GPS_HALF_MAP_CNT )
    gps_tracker_variables.half_map_size_index = 0;
  */
  
  gps_set_half_map_size_by_index(gps_tracker_variables.half_map_size_index);
}

void gps_dec_half_map_size(void)
{
  if ( gps_tracker_variables.half_map_size_index > 0 )
    gps_tracker_variables.half_map_size_index--;
  /*
  if ( gps_tracker_variables.half_map_size_index == 0 )
    gps_tracker_variables.half_map_size_index = GPS_HALF_MAP_CNT;
  gps_tracker_variables.half_map_size_index--;
  */
  
  gps_set_half_map_size_by_index(gps_tracker_variables.half_map_size_index);
}

const char *gps_get_half_map_str(void)
{
  return gps_half_map_str[gps_tracker_variables.half_map_size_index];
}


/* convert from m2_gps_pos to m2 fields */
void m2_gps_pos_to_frac_fields(void)
{
  gps_float_t f;
  
  f = gps_tracker_variables.m2_gps_pos.latitude;
  
  gps_tracker_variables.gps_frac_lat_n_s = 0;
  if ( f < 0 )
  {
    f = -f;
    gps_tracker_variables.gps_frac_lat_n_s = 1;
  }
  f *= 1000.0;
  gps_tracker_variables.gps_frac_lat = (long)f;

  f = gps_tracker_variables.m2_gps_pos.longitude;
  
  gps_tracker_variables.gps_frac_lon_e_w = 0;
  if ( f < 0 )
  {
    f = -f;
    gps_tracker_variables.gps_frac_lon_e_w = 1;
  }
  f *= 1000.0;
  gps_tracker_variables.gps_frac_lon = (long)f;  
}

/* convert from m2_gps_pos to m2 fields */
void m2_frac_fields_to_gps_pos(void)
{
  gps_float_t f;
  
  f = gps_tracker_variables.gps_frac_lat;
  f /= 1000.0;
  if ( gps_tracker_variables.gps_frac_lat_n_s != 0 )
    f = -f;  
  gps_tracker_variables.m2_gps_pos.latitude = f;

  f = gps_tracker_variables.gps_frac_lon;
  f /= 1000.0;
  if ( gps_tracker_variables.gps_frac_lon_e_w != 0 )
    f = -f;  
  gps_tracker_variables.m2_gps_pos.longitude = f;
  
}


void gps_init(void)
{
  gps_tracker_variables.cnt_10ms = 0;
  gps_set_half_map_size_by_index(1);
  
  /* init gps parser */  
  pq_Init(&pq);

  /* init uart, connect uart to gps parser */
  UART_Init(1, gps_rx);
  
}