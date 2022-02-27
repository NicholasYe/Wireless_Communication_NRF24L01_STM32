#include "NRF24L01.h"
#include "spi.h"

/*********************
 * 注：发送端的发送地址必须和接收端的接收地址一致，且发射端的应答地址为发射端的发射地址（即接收端的接收地址）
 * NRF24L01点对点运行机制：
 * 发送端发送一帧数据（这帧数据带有发送地址）、因为开启了自动应答功能，发送完毕后发送端会立即进入接收模式。
 * 此时接收端如果接到数据以后会首先会将发送端的地址和接收端的地址进行对比，如果一致（所以接收端的接收地址和发送端的发送地址要一致）
 * 则认为是正确的数据，将会接收。接收端会立即发送出自己的接收地址，这时发送端如果接收到了以后会和自己配置的接收地址比较是否一致，
 * 如果一致，则认为此次发送成功。
**********************/
const uint8_t TX_ADDRESS[TX_ADR_WIDTH] = {0x11, 0x11, 0x10, 0x10, 0x01}; //发送地址
const uint8_t RX_ADDRESS[RX_ADR_WIDTH] = {0x11, 0x11, 0x10, 0x10, 0x01}; //发送地址

const uint8_t TX_ADDRESS1[TX_ADDR] = {0xAA, 0xBB, 0xCC, 0xDD, 0x01};
const uint8_t TX_ADDRESS2[TX_ADDR] = {0xDD, 0xAA, 0xCC, 0xDD, 0x01};
const uint8_t TX_ADDRESS3[TX_ADDR] = {0x02};

const uint8_t RX_ADDRESS1[TX_ADDR] = {0xAA, 0xBB, 0xCC, 0xDD, 0x01};
const uint8_t RX_ADDRESS2[TX_ADDR] = {0xDD, 0xAA, 0xCC, 0xDD, 0x01};
const uint8_t RX_ADDRESS3[TX_ADDR] = {0x02};

//SPI2 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
uint8_t SPI2_ReadWriteByte(uint8_t TxData)
{
    uint8_t Rxdata;
    HAL_SPI_TransmitReceive(&hspi1,&TxData,&Rxdata,1, 1000);       
 	return Rxdata;          		    //返回收到的数据		
}

//初始化24L01的IO口
void NRF24L01_Init(void)
{
	NRF24L01_CE(0);  //使能24L01
	NRF24L01_CSN(1); //SPI片选取消
}
//检测24L01是否存在
//返回值:0，成功;1，失败
uint8_t NRF24L01_Check(void)
{
	uint8_t buf[5] = {0XA5, 0XA5, 0XA5, 0XA5, 0XA5};
	uint8_t i;
	// SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_4);				 //spi速度为11.25Mhz（24L01的最大SPI时钟为10Mhz,这里大一点没关系）
	NRF24L01_Write_Buf(NRF_WRITE_REG + TX_ADDR, buf, 5); //写入5个字节的地址.
	NRF24L01_Read_Buf(TX_ADDR, buf, 5);					 //读出写入的地址
	for (i = 0; i < 5; i++)
		if (buf[i] != 0XA5)
			break;
	if (i != 5)
		return 1; //检测24L01错误
	return 0;	  //检测到24L01
}
//SPI写寄存器
//reg:指定寄存器地址
//value:写入的值
uint8_t NRF24L01_Write_Reg(uint8_t reg, uint8_t value)
{
	uint8_t status;
	NRF24L01_CSN(0);				  //使能SPI传输
	status = SPI2_ReadWriteByte(reg); //发送寄存器号
	SPI2_ReadWriteByte(value);		  //写入寄存器的值
	NRF24L01_CSN(1);				  //禁止SPI传输
	return (status);				  //返回状态值
}
//读取SPI寄存器值
//reg:要读的寄存器
uint8_t NRF24L01_Read_Reg(uint8_t reg)
{
	uint8_t reg_val;
	NRF24L01_CSN(0);					//使能SPI传输
	SPI2_ReadWriteByte(reg);			//发送寄存器号
	reg_val = SPI2_ReadWriteByte(0XFF); //读取寄存器内容
	NRF24L01_CSN(1);					//禁止SPI传输
	return (reg_val);					//返回状态值
}
//在指定位置读出指定长度的数据
//reg:寄存器(位置)
//*pBuf:数据指针
//len:数据长度
//返回值,此次读到的状态寄存器值
uint8_t NRF24L01_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
	uint8_t status, uint8_t_ctr;
	NRF24L01_CSN(0);				  //使能SPI传输
	status = SPI2_ReadWriteByte(reg); //发送寄存器值(位置),并读取状态值
	for (uint8_t_ctr = 0; uint8_t_ctr < len; uint8_t_ctr++)
		pBuf[uint8_t_ctr] = SPI2_ReadWriteByte(0XFF); //读出数据
	NRF24L01_CSN(1);								  //关闭SPI传输
	return status;									  //返回读到的状态值
}
//在指定位置写指定长度的数据
//reg:寄存器(位置)
//*pBuf:数据指针
//len:数据长度
//返回值,此次读到的状态寄存器值
uint8_t NRF24L01_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
	uint8_t status, uint8_t_ctr;
	NRF24L01_CSN(0);				  //使能SPI传输
	status = SPI2_ReadWriteByte(reg); //发送寄存器值(位置),并读取状态值
	for (uint8_t_ctr = 0; uint8_t_ctr < len; uint8_t_ctr++)
		SPI2_ReadWriteByte(*pBuf++); //写入数据
	NRF24L01_CSN(1);				 //关闭SPI传输
	return status;					 //返回读到的状态值
}
//启动NRF24L01发送一次数据
//txbuf:待发送数据首地址
//返回值:发送完成状况
uint8_t NRF24L01_TxPacket(uint8_t *txbuf)
{
	uint8_t sta;
	// SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_8); //spi速度为6.75Mhz（24L01的最大SPI时钟为10Mhz）
	NRF24L01_CE(0);
	NRF24L01_Write_Buf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH); //写数据到TX BUF  32个字节
	NRF24L01_CE(1);										//启动发送
	while (NRF24L01_IRQ != 0);
													 //等待发送完成
	sta = NRF24L01_Read_Reg(STATUS);				 //读取状态寄存器的值
	NRF24L01_Write_Reg(NRF_WRITE_REG + STATUS, sta); //清除TX_DS或MAX_RT中断标志
	if (sta & MAX_TX)								 //达到最大重发次数
	{
		NRF24L01_Write_Reg(FLUSH_TX, 0xff); //清除TX FIFO寄存器
		return MAX_TX;
	}
	if (sta & TX_OK) //发送完成
	{
		return TX_OK;
	}
	return 0xff; //其他原因发送失败
}
//启动NRF24L01发送一次数据
//txbuf:待发送数据首地址
//返回值:0，接收完成；其他，错误代码
uint8_t NRF24L01_RxPacket(uint8_t *rxbuf)
{
	uint8_t sta;
	sta = NRF24L01_Read_Reg(STATUS);				 //读取状态寄存器的值
	NRF24L01_Write_Reg(NRF_WRITE_REG + STATUS, sta); //清除TX_DS或MAX_RT中断标志
	if (sta & RX_OK)								 //接收到数据
	{
		NRF24L01_Read_Buf(RD_RX_PLOAD, rxbuf, RX_PLOAD_WIDTH); //读取数据
		NRF24L01_Write_Reg(FLUSH_RX, 0xff);					   //清除RX FIFO寄存器
		return 0;
	}
	return 1; //没收到任何数据
}
//该函数初始化NRF24L01到RX模式
//设置RX地址,写RX数据宽度,选择RF频道,波特率和LNA HCURR
//当CE变高后,即进入RX模式,并可以接收数据了
void NRF24L01_RX_Mode(void)
{
	NRF24L01_CE(0);
	NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, (uint8_t *)RX_ADDRESS, RX_ADR_WIDTH); //写RX节点地址

	NRF24L01_Write_Reg(NRF_WRITE_REG + EN_AA, 0x01);			  //使能通道0的自动应答
	NRF24L01_Write_Reg(NRF_WRITE_REG + EN_RXADDR, 0x01);		  //使能通道0的接收地址
	NRF24L01_Write_Reg(NRF_WRITE_REG + RF_CH, 40);				  //设置RF通信频率
	NRF24L01_Write_Reg(NRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //选择通道0的有效数据宽度
	NRF24L01_Write_Reg(NRF_WRITE_REG + RF_SETUP, 0x0f);			  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启
	NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0f);			  //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式
	NRF24L01_CE(1);											  //CE为高,进入接收模式
}

//该函数初始化NRF24L01到TX模式
//设置TX地址,写TX数据宽度,设置RX自动应答的地址,填充TX发送数据,选择RF频道,波特率和LNA HCURR
//PWR_UP,CRC使能
//当CE变高后,即进入RX模式,并可以接收数据了
//CE为高大于10us,则启动发送.
void NRF24L01_TX_Mode(void)
{
	NRF24L01_CE(0);
	NRF24L01_Write_Buf(NRF_WRITE_REG + TX_ADDR, (uint8_t *)TX_ADDRESS1, TX_ADR_WIDTH);	 //写TX节点地址
	NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, (uint8_t *)RX_ADDRESS1, RX_ADR_WIDTH); //设置TX节点地址,主要为了使能ACK

	NRF24L01_Write_Reg(NRF_WRITE_REG + EN_AA, 0x3f);	  //使能通道0的自动应答
	NRF24L01_Write_Reg(NRF_WRITE_REG + EN_RXADDR, 0x3f);  //使能通道0的接收地址
	NRF24L01_Write_Reg(NRF_WRITE_REG + SETUP_RETR, 0xff); //设置自动重发间隔时间:(0x1a 500us + 86us;最大自动重发次数:10次)  (4000us + 86us 15次)
	NRF24L01_Write_Reg(NRF_WRITE_REG + RF_CH, 40);		  //设置RF通道为40	2.4G + 40Mhz = 2.44G
	NRF24L01_Write_Reg(NRF_WRITE_REG + RF_SETUP, 0x0f);	  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启
	NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0e);	  //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
	NRF24L01_CE(1);									  //CE为高,10us后启动发送
}

//设置发射端配置
void NRF_Set_Mode_Tx(uint8_t *addr)
{
	NRF24L01_CE(0);
	NRF24L01_Write_Buf(NRF_WRITE_REG + TX_ADDR, addr, TX_ADR_WIDTH);	 //写TX节点地址
	NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, addr, RX_ADR_WIDTH);  //设置TX节点地址,主要为了使能ACK

	NRF24L01_Write_Reg(NRF_WRITE_REG + EN_AA, 0x3f);	  //使能通道0的自动应答
	NRF24L01_Write_Reg(NRF_WRITE_REG + EN_RXADDR, 0x3f);  //使能通道0的接收地址
	NRF24L01_Write_Reg(NRF_WRITE_REG + SETUP_RETR, 0xff); //设置自动重发间隔时间:(0x1a 500us + 86us;最大自动重发次数:10次)  (4000us + 86us 15次)
	NRF24L01_Write_Reg(NRF_WRITE_REG + RF_CH, 40);		  //设置RF通道为40
	NRF24L01_Write_Reg(NRF_WRITE_REG + RF_SETUP, 0x0f);	  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启
	NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0e);	  //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
	NRF24L01_CE(1);									  //CE为高,10us后启动发送
}

//设置发射端配置
void NRF_Set_Mode_Rx(uint8_t *addr)
{
	NRF24L01_CE(0);
	NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, addr, RX_ADR_WIDTH); //写RX节点地址

	NRF24L01_Write_Reg(NRF_WRITE_REG + EN_AA, 0x3f);			  //使能通道0的自动应答
	NRF24L01_Write_Reg(NRF_WRITE_REG + EN_RXADDR, 0x3f);		  //使能通道0的接收地址
	NRF24L01_Write_Reg(NRF_WRITE_REG + RF_CH, 40);				  //设置RF通信频率
	NRF24L01_Write_Reg(NRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //选择通道0的有效数据宽度
	NRF24L01_Write_Reg(NRF_WRITE_REG + RF_SETUP, 0x0f);			  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启
	NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0f);			  //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式
	NRF24L01_CE(1);									  //CE为高,10us后启动发送
}


//设置NRF24L01工作模式
void NRF24L01_Set_Mode(__NRFRESULT _mode, uint8_t *addr)
{
	
	if(_mode == NRF_MODE_TX){
		
	}
	else if(_mode == NRF_MODE_RX){
		
	}
	
}

//NRF24L01发送数函数
//数据长度自定义且不能超过32字节
__NRFRESULT NRF24L01_TxData(uint8_t *_txvar, uint8_t len)
{
	uint8_t sta;
	NRF24L01_CE(0);
	if(len < TX_PLOAD_MAX_WIDTH){		//小于最大发送字节
		NRF24L01_Write_Buf(WR_TX_PLOAD, _txvar, len); //写数据到TX BUF  32个字节
		NRF24L01_CE(1);										//启动发送
		while (NRF24L01_IRQ != 0);		//等待发送完成
		sta = NRF24L01_Read_Reg(STATUS);				 //读取状态寄存器的值
		NRF24L01_Write_Reg(NRF_WRITE_REG + STATUS, sta); //清除TX_DS或MAX_RT中断标志
		if (sta & MAX_TX){							 	//达到最大重发次数
			NRF24L01_Write_Reg(FLUSH_TX, 0xff); //清除TX FIFO寄存器
			return (__NRFRESULT)MAX_TX;
		}
		if (sta & TX_OK){ //发送完成
			return NRF_OK;
		}		
	}
	return NRF_ERROR; //其他原因发送失败		
}


