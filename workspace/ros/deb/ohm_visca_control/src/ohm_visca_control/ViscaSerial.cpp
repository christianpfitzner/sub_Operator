/*
 * ViscaSerial.cpp
 *
 *  Created on: 07.12.2018
 *      Author: volletjo
 */
#include <iostream>
#include "ViscaSerial.h"

/********************************/
/*      PRIVATE FUNCTIONS       */
/********************************/

void ViscaClass::append_byte(VISCAPacket_t *packet, unsigned char byte)
{
  packet->bytes[packet->length]=byte;
  (packet->length)++;
}

void ViscaClass::init_packet(VISCAPacket_t *packet)
{
  // we start writing at byte 1, the first byte will be filled by the
  // packet sending function. This function will also append a terminator.
  packet->length=1;
}

uint32_t ViscaClass::write_packet_data(VISCAInterface_t *iface, VISCACamera_t *camera, VISCAPacket_t *packet)
{
  int err;

  err = write(iface->port_fd, packet->bytes, packet->length);
  if ( err < packet->length )
    return VISCA_FAILURE;
  else
    return VISCA_SUCCESS;
}
//TODO
uint32_t ViscaClass::get_packet(VISCAInterface_t *iface)
{
  int pos=0;
  int bytes_read;
  // wait for message
  ioctl(iface->port_fd, FIONREAD, &(iface->bytes));
  int i = 0;
  while (iface->bytes==0) {
    std::this_thread::sleep_for (std::chrono::microseconds(1));

    ioctl(iface->port_fd, FIONREAD, &(iface->bytes));
    i++;
    if (i==2000)
    {
      return VISCA_FAILURE;
      std::cout<<"pupkack"<<std::endl;
    }
  }

  // get octets one by one
  bytes_read=read(iface->port_fd, iface->ibuf, 1);
  while (iface->ibuf[pos]!=CAM_Terminator) {
    pos++;
    bytes_read=read(iface->port_fd, &iface->ibuf[pos], 1);
//    std::this_thread::sleep_for (std::chrono::microseconds(1));

  }
  iface->bytes=pos+1;
  return VISCA_SUCCESS;
}
uint32_t ViscaClass::get_reply(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  // first message: -------------------
  if (get_packet(iface)!=VISCA_SUCCESS)
    return VISCA_FAILURE;
  iface->type=iface->ibuf[1]&0xF0;
  // skip ack messages
  while (iface->type==CAM_RESPONSE_ACK)
  {
    if (get_packet(iface)!=VISCA_SUCCESS)
      return VISCA_FAILURE;
    iface->type=iface->ibuf[1]&0xF0;
  }
  switch (iface->type)
  {
  case CAM_RESPONSE_CLEAR:
    return VISCA_SUCCESS;
    break;
  case CAM_RESPONSE_ADDRESS:
    return VISCA_SUCCESS;
    break;
  case CAM_RESPONSE_COMPLETED:
    return VISCA_SUCCESS;
    break;
  case CAM_RESPONSE_ERROR:
    return VISCA_SUCCESS;
    break;
  }
  return VISCA_FAILURE;
}

uint32_t ViscaClass::send_packet(VISCAInterface_t *iface, VISCACamera_t *camera, VISCAPacket_t *packet)
{
  // check data:
  if ((iface->address>7)||(camera->address>7)||(iface->broadcast>1))
  {
#if DEBUG
    fprintf(stderr,"(%s): Invalid header parameters\n",__FILE__);
    fprintf(stderr," %d %d %d   \n",iface->address,camera->address,iface->broadcast);
#endif
    return VISCA_FAILURE;
  }

  // build header:
  packet->bytes[0]=0x80;
  packet->bytes[0]|=(iface->address << 4);
  if (iface->broadcast>0)
  {
    packet->bytes[0]|=(iface->broadcast << 3);
    packet->bytes[0]&=0xF8;
  }
  else
    packet->bytes[0]|=camera->address;

  // append footer
  append_byte(packet,CAM_Terminator);

  return write_packet_data(iface,camera,packet);
}

uint32_t ViscaClass::send_packet_with_reply(VISCAInterface_t *iface, VISCACamera_t *camera, VISCAPacket_t *packet)
{

  if (send_packet(iface,camera,packet)!=VISCA_SUCCESS)
    return VISCA_FAILURE;

  if (get_reply(iface,camera)!=VISCA_SUCCESS)
    return VISCA_FAILURE;

  return VISCA_SUCCESS;
}

/****************************************************************************/
/*                           PUBLIC FUNCTIONS                               */
/****************************************************************************/

uint32_t ViscaClass::open_serial(VISCAInterface_t *iface, const char *device_name)
{
  int fd;
  //const char *device = "/dev/ttyACM0";
//  fd = open("/dev/ttyACM0", O_RDWR | O_NDELAY | O_NOCTTY);
  fd = open(device_name, O_RDWR | O_NDELAY | O_NOCTTY);
  if (fd == -1)
  {

    fprintf(stderr,"(%s): cannot open serial device %s\n",__FILE__,device_name);

    iface->port_fd=-1;
    return VISCA_FAILURE;
  }
  else
  {
    fcntl(fd, F_SETFL,0);
    /* Setting port parameters */
    tcgetattr(fd, &iface->options);

    /* control flags */
    cfsetispeed(&iface->options,B9600);    /* 9600 Bds   */
    iface->options.c_cflag &= ~PARENB;     /* No parity  */
    iface->options.c_cflag &= ~CSTOPB;     /*            */
    iface->options.c_cflag &= ~CSIZE;      /* 8bit       */
    iface->options.c_cflag |= CS8;         /*            */
    iface->options.c_cflag &= ~CRTSCTS;    /* No hdw ctl */

    /* local flags */
    iface->options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); /* raw input */

    /* input flags */
    /*
      iface->options.c_iflag &= ~(INPCK | ISTRIP); // no parity
      iface->options.c_iflag &= ~(IXON | IXOFF | IXANY); // no soft ctl
     */
    /* patch: bpflegin: set to 0 in order to avoid invalid pan/tilt return values */
    iface->options.c_iflag = 0;

    /* output flags */
    iface->options.c_oflag &= ~OPOST; /* raw output */

    tcsetattr(fd, TCSANOW, &iface->options);

  }
  iface->port_fd = fd;
  iface->address=0;

  return VISCA_SUCCESS;
}

uint32_t ViscaClass::close_serial(VISCAInterface_t *iface)
{
  if (iface->port_fd!=-1)
  {
    close(iface->port_fd);
    iface->port_fd = -1;
    return VISCA_SUCCESS;
  }
  else
    return VISCA_FAILURE;
}

uint32_t ViscaClass::unread_bytes(VISCAInterface_t *iface, unsigned char *buffer, uint32_t *buffer_size)
{
  uint32_t bytes = 0;
  *buffer_size = 0;

  ioctl(iface->port_fd, FIONREAD, &bytes);
  if (bytes>0)
  {
    bytes = (bytes>*buffer_size) ? *buffer_size : bytes;
    read(iface->port_fd, &buffer, bytes);
    *buffer_size = bytes;
    return VISCA_FAILURE;
  }
  return VISCA_SUCCESS;
}



uint32_t ViscaClass::usleep(uint32_t useconds)
{
  std::this_thread::sleep_for (std::chrono::microseconds(useconds));
  return VISCA_SUCCESS;
 // return (uint32_t) usleep(useconds);
}

uint32_t ViscaClass::set_address(VISCAInterface_t *iface, int *camera_num)
{
  VISCAPacket_t packet;
  int backup;
  VISCACamera_t camera; /* dummy camera struct */

  camera.address=0;
  backup=iface->broadcast;

  init_packet(&packet);
  append_byte(&packet,0x30);
  append_byte(&packet,0x01);

  iface->broadcast=1;
  if (send_packet(iface, &camera, &packet)!=VISCA_SUCCESS)
  {
    iface->broadcast=backup;
    return VISCA_FAILURE;
  }
  else
    iface->broadcast=backup;

  if (get_reply(iface, &camera)!=VISCA_SUCCESS)
    return VISCA_FAILURE;
  else
  {
    /* We parse the message from the camera here  */
    /* We expect to receive 4*camera_num bytes,
         every packet should be 88 30 0x FF, x being
         the camera id+1. The number of cams will thus be
         ibuf[bytes-2]-1  */

    if ((iface->bytes & 0x3)!=0) /* check multiple of 4 */
      return VISCA_FAILURE;
    else
    {
      *camera_num=iface->ibuf[iface->bytes-2]-1;
      if ((*camera_num==0)||(*camera_num>7))
        return VISCA_FAILURE;
      else
        return VISCA_SUCCESS;
    }
  }
}

uint32_t ViscaClass::clear(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet,0x01);
  append_byte(&packet,0x00);
  append_byte(&packet,0x01);

  if (send_packet(iface, camera, &packet)!=VISCA_SUCCESS)
    return VISCA_FAILURE;
  else
    if (get_reply(iface, camera)!=VISCA_SUCCESS)
      return VISCA_FAILURE;
    else
      return VISCA_SUCCESS;
}

uint32_t ViscaClass::get_version(VISCAInterface_t *iface, VISCACamera_t *camera)
{

  VISCAPacket_t packet;
  packet.bytes[0]=0x80 | camera->address;
  packet.bytes[1]=0x09;
  packet.bytes[2]=0x00;
  packet.bytes[3]=0x02;
  packet.bytes[4]=CAM_Terminator;
  packet.length=5;


  if (write_packet_data(iface, camera, &packet)!=VISCA_SUCCESS)
    return VISCA_FAILURE;
  else
    if (get_reply(iface, camera)!=VISCA_SUCCESS)
      return VISCA_FAILURE;

  if (iface->bytes!= 10) /* we expect 10 bytes as answer */
    return VISCA_FAILURE;
  else
  {
    camera->vendor=(iface->ibuf[2]<<8) + iface->ibuf[3];
    camera->model=(iface->ibuf[4]<<8) + iface->ibuf[5];
    camera->rom_version=(iface->ibuf[6]<<8) + iface->ibuf[7];
    camera->socket_num=iface->ibuf[8];
    return VISCA_SUCCESS;
  }
}

uint32_t ViscaClass::set_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Power);
  append_byte(&packet, CAM_Power_Off);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_zoom_stop(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Zoom);
  append_byte(&packet, CAM_Zoom_Stop);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_zoom_tele(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Zoom);
  append_byte(&packet, CAM_Zoom_Tele);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_zoom_wide(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Zoom);
  append_byte(&packet, CAM_Zoom_Wide);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_zoom_tele_speed(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t speed)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Zoom);
  append_byte(&packet, CAM_Zoom_Tele_VarSpeed | (speed & 0x7));

  return send_packet_with_reply(iface, camera, &packet);
}


uint32_t ViscaClass::set_zoom_wide_speed(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t speed)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Zoom);
  append_byte(&packet, CAM_Zoom_Wide_VarSpeed | (speed & 0x7));

  return send_packet_with_reply(iface, camera, &packet);

}

uint32_t ViscaClass::set_zoom_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t zoom)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Zoom_Direct);
  append_byte(&packet, (zoom & 0xF000) >> 12);
  append_byte(&packet, (zoom & 0x0F00) >>  8);
  append_byte(&packet, (zoom & 0x00F0) >>  4);
  append_byte(&packet, (zoom & 0x000F));

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_digital_zoom_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Dzoom);
  append_byte(&packet, power);

  return send_packet_with_reply(iface, camera, &packet);
}


uint32_t ViscaClass::set_focus_stop(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Focus);
  append_byte(&packet, CAM_Focus_Stop);

  return send_packet_with_reply(iface, camera, &packet);
}


uint32_t ViscaClass::set_focus_far(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Focus);
  append_byte(&packet, CAM_Focus_Far);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_focus_near(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Focus);
  append_byte(&packet, CAM_Focus_Near);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_focus_far_speed(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t speed)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Focus);
  append_byte(&packet, CAM_Focus_Far_VarSpeed | (speed & 0x7));

  return send_packet_with_reply(iface, camera, &packet);
}


uint32_t ViscaClass::set_focus_near_speed(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t speed)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Focus);
  append_byte(&packet, CAM_Focus_Near_VarSpeed | (speed & 0x7));

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_focus_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t focus)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Focus_Direct);
  append_byte(&packet, (focus & 0xF000) >> 12);
  append_byte(&packet, (focus & 0x0F00) >>  8);
  append_byte(&packet, (focus & 0x00F0) >>  4);
  append_byte(&packet, (focus & 0x000F));

  return send_packet_with_reply(iface, camera, &packet);
}


uint32_t ViscaClass::set_focus_auto(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Focus_Auto);
  append_byte(&packet, CAM_Focus_Auto_On);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_focus_manual(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Focus_Auto);
  append_byte(&packet, CAM_Focus_Auto_Off);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_focus_toggle(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Focus_Auto);
  append_byte(&packet, CAM_Focus_Auto_Toggle);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_focus_one_push(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Focus_One_Push);
  append_byte(&packet, CAM_Focus_One_Push_Trigger);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_focus_near_limit(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t limit)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Focus_Near_Limit);
  append_byte(&packet, (limit & 0xF000) >> 12);
  append_byte(&packet, (limit & 0x0F00) >>  8);
  append_byte(&packet, (limit & 0x00F0) >>  4);
  append_byte(&packet, (limit & 0x000F));

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_focus_autosense_normal(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_AF_Sensitivity);
  append_byte(&packet, CAM_AF_Sensitivity_Normal);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_focus_autosense_low(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_AF_Sensitivity);
  append_byte(&packet, CAM_AF_Sensitivity_Low);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_afmode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t mode)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_AFMode);
  append_byte(&packet, mode);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_afmode_time(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t movement_time, uint8_t interval_time)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_AFMode_Time);
  append_byte(&packet, (movement_time & 0xF0) >> 4);
  append_byte(&packet, (movement_time & 0x0F) );
  append_byte(&packet, (interval_time & 0xF0) >>  4);
  append_byte(&packet, (interval_time & 0x0F));

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_zoom_and_focus_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t zoom, uint16_t focus)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_ZoomFocus_Direct);
  append_byte(&packet, (zoom & 0xF000) >> 12);
  append_byte(&packet, (zoom & 0x0F00) >>  8);
  append_byte(&packet, (zoom & 0x00F0) >>  4);
  append_byte(&packet, (zoom & 0x000F));
  append_byte(&packet, (focus & 0xF000) >> 12);
  append_byte(&packet, (focus & 0x0F00) >>  8);
  append_byte(&packet, (focus & 0x00F0) >>  4);
  append_byte(&packet, (focus & 0x000F));

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_vibration_compensation_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_VibrationCompensation);
  append_byte(&packet, power);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_whitebalance_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t mode)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_WB);
  append_byte(&packet, mode);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_whitebalance_one_push(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_WB_One_Push);
  append_byte(&packet, CAM_WB_One_Push_Trigger);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_rgain_reset(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_RGain);
  append_byte(&packet, CAM_RGain_Reset);

  return send_packet_with_reply(iface, camera, &packet);

}

uint32_t ViscaClass::set_rgain_up(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_RGain);
  append_byte(&packet, CAM_RGain_Up);

  return send_packet_with_reply(iface, camera, &packet);
}


uint32_t ViscaClass::set_rgain_down(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_RGain);
  append_byte(&packet, CAM_RGain_Down);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_rgain_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_RGain_Direct);
  append_byte(&packet, (value & 0xF000) >> 12);
  append_byte(&packet, (value & 0x0F00) >>  8);
  append_byte(&packet, (value & 0x00F0) >>  4);
  append_byte(&packet, (value & 0x000F));

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_bgain_reset(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_BGain);
  append_byte(&packet, CAM_BGain_Reset);

  return send_packet_with_reply(iface, camera, &packet);

}

uint32_t ViscaClass::set_bgain_up(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_BGain);
  append_byte(&packet, CAM_BGain_Up);

  return send_packet_with_reply(iface, camera, &packet);
}


uint32_t ViscaClass::set_bgain_down(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_BGain);
  append_byte(&packet, CAM_BGain_Down);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_bgain_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_BGain_Direct);
  append_byte(&packet, (value & 0xF000) >> 12);
  append_byte(&packet, (value & 0x0F00) >>  8);
  append_byte(&packet, (value & 0x00F0) >>  4);
  append_byte(&packet, (value & 0x000F));

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_auto_exp_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t mode)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_AE);
  append_byte(&packet, mode);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_slow_shutter_auto(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_SlowShutter);
  append_byte(&packet, power);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_shutter_reset(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Shutter);
  append_byte(&packet, CAM_Shutter_Reset);

  return send_packet_with_reply(iface, camera, &packet);

}

uint32_t ViscaClass::set_shutter_up(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Shutter);
  append_byte(&packet, CAM_Shutter_Up);

  return send_packet_with_reply(iface, camera, &packet);
}


uint32_t ViscaClass::set_shutter_down(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Shutter);
  append_byte(&packet, CAM_Shutter_Down);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_shutter_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Shutter_Direct);
  append_byte(&packet, (value & 0xF000) >> 12);
  append_byte(&packet, (value & 0x0F00) >>  8);
  append_byte(&packet, (value & 0x00F0) >>  4);
  append_byte(&packet, (value & 0x000F));

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_iris_reset(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Iris);
  append_byte(&packet, CAM_Iris_Reset);

  return send_packet_with_reply(iface, camera, &packet);

}

uint32_t ViscaClass::set_iris_up(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Iris);
  append_byte(&packet, CAM_Iris_Up);

  return send_packet_with_reply(iface, camera, &packet);
}


uint32_t ViscaClass::set_iris_down(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Iris);
  append_byte(&packet, CAM_Iris_Down);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_iris_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Iris_Direct);
  append_byte(&packet, (value & 0xF000) >> 12);
  append_byte(&packet, (value & 0x0F00) >>  8);
  append_byte(&packet, (value & 0x00F0) >>  4);
  append_byte(&packet, (value & 0x000F));

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_gain_reset(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Gain);
  append_byte(&packet, CAM_Gain_Reset);

  return send_packet_with_reply(iface, camera, &packet);

}

uint32_t ViscaClass::set_gain_up(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Gain);
  append_byte(&packet, CAM_Gain_Up);

  return send_packet_with_reply(iface, camera, &packet);
}


uint32_t ViscaClass::set_gain_down(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Gain);
  append_byte(&packet, CAM_Gain_Down);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_gain_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Gain_Direct);
  append_byte(&packet, (value & 0xF000) >> 12);
  append_byte(&packet, (value & 0x0F00) >>  8);
  append_byte(&packet, (value & 0x00F0) >>  4);
  append_byte(&packet, (value & 0x000F));

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_gain_limit(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t value)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Gain_Limit);
  append_byte(&packet, value);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_exp_comp_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_ExpComp_Enable);
  append_byte(&packet, power);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_exp_comp_reset(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_ExpComp);
  append_byte(&packet, CAM_ExpComp_Reset);

  return send_packet_with_reply(iface, camera, &packet);

}

uint32_t ViscaClass::set_exp_comp_up(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_ExpComp);
  append_byte(&packet, CAM_ExpComp_Up);

  return send_packet_with_reply(iface, camera, &packet);
}


uint32_t ViscaClass::set_exp_comp_down(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_ExpComp);
  append_byte(&packet, CAM_ExpComp_Down);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_exp_comp_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_ExpComp_Direct);
  append_byte(&packet, (value & 0xF000) >> 12);
  append_byte(&packet, (value & 0x0F00) >>  8);
  append_byte(&packet, (value & 0x00F0) >>  4);
  append_byte(&packet, (value & 0x000F));

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_backlight_comp_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_BackLightModeInq);
  append_byte(&packet, power);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_spot_ae_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_SpotAE);
  append_byte(&packet, power);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_spot_ae_position(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t x_position, uint8_t y_position)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_SpotAE_Position);
  append_byte(&packet, (x_position & 0xF0) >>  4);
  append_byte(&packet, (x_position & 0x0F));
  append_byte(&packet, (y_position & 0xF0) >>  4);
  append_byte(&packet, (y_position & 0x0F));

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_ae_response_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t value)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_AE_Response_Direct);
  append_byte(&packet, value);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_wide_dynamic_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_WD);
  append_byte(&packet, power);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_defog_on(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t value)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Defog);
  append_byte(&packet, CAM_Defog_On);
  append_byte(&packet, (value & 3));

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_defog_off(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Defog);
  append_byte(&packet, CAM_Defog_Off);
  append_byte(&packet, 0x00);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_aperture_reset(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Aperture);
  append_byte(&packet, CAM_Aperture_Reset);

  return send_packet_with_reply(iface, camera, &packet);

}

uint32_t ViscaClass::set_aperture_up(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Aperture);
  append_byte(&packet, CAM_Aperture_Up);

  return send_packet_with_reply(iface, camera, &packet);
}


uint32_t ViscaClass::set_aperture_down(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Aperture);
  append_byte(&packet, CAM_Aperture_Down);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_aperture_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Aperture_Direct);
  append_byte(&packet, (value & 0xF000) >> 12);
  append_byte(&packet, (value & 0x0F00) >>  8);
  append_byte(&packet, (value & 0x00F0) >>  4);
  append_byte(&packet, (value & 0x000F));

  return send_packet_with_reply(iface, camera, &packet);
}


uint32_t ViscaClass::set_noise_reduction(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t value3DNR, uint8_t value2DNR)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_NR);
  append_byte(&packet, ((value3DNR << 4) | value2DNR));

  return send_packet_with_reply(iface, camera, &packet);
}


uint32_t ViscaClass::set_gamma(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t value)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Gamma);
  append_byte(&packet, (value & 3));

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_lr_reverse_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_LR_Reverse);
  append_byte(&packet, power);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_freeze_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_FREEZE);
  append_byte(&packet, power);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_picture_effect(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_PictureEffect);
  append_byte(&packet, power);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_pictureflip_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_PictureFlip);
  append_byte(&packet, power);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::memory_reset(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t channel)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Memory);
  append_byte(&packet, CAM_Memory_Reset);
  append_byte(&packet, channel);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::memory_set(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t channel)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Memory);
  append_byte(&packet, CAM_Memory_Set);
  append_byte(&packet, channel);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::memory_recall(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t channel)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Memory);
  append_byte(&packet, CAM_Memory_Set);
  append_byte(&packet, channel);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_mute_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Mute);
  append_byte(&packet, power);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_continuous_zoom_pos_reply_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_ContinuousZoomPosReply);
  append_byte(&packet, power);

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_reply_interval_time_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t value)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_ReplyIntervalTimeSet);
  append_byte(&packet, 0x00);
  append_byte(&packet, 0x00);
  append_byte(&packet, (value & 0xF0) >>  4);
  append_byte(&packet, (value & 0x0F));

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_register(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t reg_num, uint8_t reg_val)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_RegisterValue);
  append_byte(&packet, reg_num);
  append_byte(&packet, (reg_val & 0xF0) >>  4);
  append_byte(&packet, (reg_val & 0x0F));
  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_chroma_suppression(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t value)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_ChromaSuppress);
  append_byte(&packet, (value & 3));

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_color_gain_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_ColorGain_Direct);
  append_byte(&packet, (value & 0xF000) >> 12);
  append_byte(&packet, (value & 0x0F00) >>  8);
  append_byte(&packet, (value & 0x00F0) >>  4);
  append_byte(&packet, (value & 0x000F));

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::set_color_hue_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value)
{
  VISCAPacket_t packet;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_ColorHue_Direct);
  append_byte(&packet, (value & 0xF000) >> 12);
  append_byte(&packet, (value & 0x0F00) >>  8);
  append_byte(&packet, (value & 0x00F0) >>  4);
  append_byte(&packet, (value & 0x000F));

  return send_packet_with_reply(iface, camera, &packet);
}

uint32_t ViscaClass::get_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Power);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}

uint32_t ViscaClass::get_dzoom(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Dzoom);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}

uint32_t ViscaClass::get_zoom_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;


  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Zoom_Direct);

  err=send_packet_with_reply(iface, camera, &packet);

  if (err!=VISCA_SUCCESS)
  {
    return err;
  }
  else
  {
    *value=(iface->ibuf[2]<<12)+(iface->ibuf[3]<<8)+(iface->ibuf[4]<<4)+iface->ibuf[5];
    return VISCA_SUCCESS;
  }
}

uint32_t ViscaClass::get_focus_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Focus_Direct);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
  {
    *value=(iface->ibuf[2]<<12)+(iface->ibuf[3]<<8)+(iface->ibuf[4]<<4)+iface->ibuf[5];
    return VISCA_SUCCESS;
  }
}

uint32_t ViscaClass::get_focus_auto(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Focus_Auto);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}

uint32_t ViscaClass::get_focus_near_limit(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Focus_Near_Limit);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
  {
    *value=(iface->ibuf[2]<<12)+(iface->ibuf[3]<<8)+(iface->ibuf[4]<<4)+iface->ibuf[5];
    return VISCA_SUCCESS;
  }
}

uint32_t ViscaClass::get_focus_auto_sensitivity(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *mode)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_AF_Sensitivity);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *mode=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}

uint32_t ViscaClass::get_afmode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *mode)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_AFMode);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *mode=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}

uint32_t ViscaClass::get_afmode_time(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *movement_time, uint8_t *interval_time)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_AFMode_Time);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
  {
    *movement_time=(iface->ibuf[2]<<4)+iface->ibuf[5];
    *interval_time=(iface->ibuf[4]<<4)+iface->ibuf[5];
    return VISCA_SUCCESS;
  }
}

uint32_t ViscaClass::get_whitebalance_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *mode)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_WB);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *mode=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}

uint32_t ViscaClass::get_rgain_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_RGain_Direct);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
  {
    *value=(iface->ibuf[2]<<12)+(iface->ibuf[3]<<8)+(iface->ibuf[4]<<4)+iface->ibuf[5];
    return VISCA_SUCCESS;
  }
}

uint32_t ViscaClass::get_bgain_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_BGain_Direct);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
  {
    *value=(iface->ibuf[2]<<12)+(iface->ibuf[3]<<8)+(iface->ibuf[4]<<4)+iface->ibuf[5];
    return VISCA_SUCCESS;
  }
}

uint32_t ViscaClass::get_auto_exp_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *mode)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_AE);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *mode=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}

uint32_t ViscaClass::get_slow_shutter_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *mode)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_SlowShutter);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *mode=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}

uint32_t ViscaClass::get_shutter_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Shutter_Direct);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
  {
    *value=(iface->ibuf[2]<<12)+(iface->ibuf[3]<<8)+(iface->ibuf[4]<<4)+iface->ibuf[5];
    return VISCA_SUCCESS;
  }
}

uint32_t ViscaClass::get_iris_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Iris_Direct);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
  {
    *value=(iface->ibuf[2]<<12)+(iface->ibuf[3]<<8)+(iface->ibuf[4]<<4)+iface->ibuf[5];
    return VISCA_SUCCESS;
  }
}

uint32_t ViscaClass::get_gain_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Gain_Direct);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
  {
    *value=(iface->ibuf[2]<<12)+(iface->ibuf[3]<<8)+(iface->ibuf[4]<<4)+iface->ibuf[5];
    return VISCA_SUCCESS;
  }
}

uint32_t ViscaClass::get_gain_limit(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Gain_Limit);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
  {
    *value=(iface->ibuf[2]);
    return VISCA_SUCCESS;
  }
}

uint32_t ViscaClass::get_exp_comp_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_ExpComp_Enable);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}

uint32_t ViscaClass::get_exp_comp_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_ExpComp_Direct);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
  {
    *value=(iface->ibuf[2]<<12)+(iface->ibuf[3]<<8)+(iface->ibuf[4]<<4)+iface->ibuf[5];
    return VISCA_SUCCESS;
  }
}

uint32_t ViscaClass::get_backlight_comp_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_BackLightModeInq);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}

uint32_t ViscaClass::get_spot_ae_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_SpotAE);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}

uint32_t ViscaClass::get_spot_ae_position(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *x_position, uint8_t *y_position)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_SpotAE_Position);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
  {
    *x_position=(iface->ibuf[2]<<4)+iface->ibuf[3];
    *y_position=(iface->ibuf[4]<<4)+iface->ibuf[5];
    return VISCA_SUCCESS;
  }
}

uint32_t ViscaClass::get_ae_response_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_AE_Response_Direct);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
  {
    *value=(iface->ibuf[2]);
    return VISCA_SUCCESS;
  }
}

uint32_t ViscaClass::get_wide_dynamic_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_WD);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}

uint32_t ViscaClass::get_defog_on(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Defog);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}

uint32_t ViscaClass::get_aperture_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Aperture);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
  {
    *value=(iface->ibuf[4]<<4)+iface->ibuf[5];
    return VISCA_SUCCESS;
  }
}

uint32_t ViscaClass::get_noise_reduction(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value3DNR, uint8_t *value2DNR)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_NR);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
  {
    *value3DNR=((iface->ibuf[2] & 0xF0)>>4);
    *value2DNR=(iface->ibuf[2] & 0x0F);
    return VISCA_SUCCESS;
  }
}

uint32_t ViscaClass::get_gamma(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Gamma);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
  {
    *value=iface->ibuf[2];
    return VISCA_SUCCESS;
  }
}

uint32_t ViscaClass::get_lr_reverse_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_LR_Reverse);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}

uint32_t ViscaClass::get_FREEZE_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_FREEZE);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}

uint32_t ViscaClass::get_picture_effect(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *mode)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_PictureEffect);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *mode=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}

uint32_t ViscaClass::get_pictureflip_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_PictureFlip);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}

uint32_t ViscaClass::get_memory(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *channel)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Memory);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *channel=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}

uint32_t ViscaClass::get_mute_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Mute);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}

uint32_t ViscaClass::get_continuous_zoom_pos_reply_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_ContinuousZoomPosReply);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}

uint32_t ViscaClass::get_reply_interval_time_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_ReplyIntervalTimeSet);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
  {
    *value=(iface->ibuf[4]<<4)+iface->ibuf[5];
    return VISCA_SUCCESS;
  }
}

uint32_t ViscaClass::get_register(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t reg_num, uint8_t* reg_val)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Command);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_RegisterValue);
  append_byte(&packet, reg_num);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *reg_val=(iface->ibuf[2]<<4)+iface->ibuf[3];
      return VISCA_SUCCESS;
    }
}

uint32_t ViscaClass::get_chroma_suppression(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_ChromaSuppress);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *value=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}

uint32_t ViscaClass::get_color_gain_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_ColorGain_Direct);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *value=iface->ibuf[5];
      return VISCA_SUCCESS;
    }
}

uint32_t ViscaClass::get_color_hue_direct(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_ColorHue_Direct);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *value=iface->ibuf[5];
      return VISCA_SUCCESS;
    }
}

uint32_t ViscaClass::get_temperature(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_Lens_Temp);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
  {
    *value=(iface->ibuf[4]<<4)+iface->ibuf[5];
    return VISCA_SUCCESS;
  }
}

uint32_t ViscaClass::get_vibration_compensation_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  init_packet(&packet);
  append_byte(&packet, CAM_Inquiry);
  append_byte(&packet, CAM_Category_Camera1);
  append_byte(&packet, CAM_VibrationCompensation);
  err=send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=(iface->ibuf[4]<<4)+iface->ibuf[5];
      return VISCA_SUCCESS;
    }
}

ViscaClass viscaSerial;
