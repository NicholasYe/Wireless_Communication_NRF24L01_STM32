#include "NRF24L01.h"
#include "spi.h"

/*********************
 * ע�����Ͷ˵ķ��͵�ַ����ͽ��ն˵Ľ��յ�ַһ�£��ҷ���˵�Ӧ���ַΪ����˵ķ����ַ�������ն˵Ľ��յ�ַ��
 * NRF24L01��Ե����л��ƣ�
 * ���Ͷ˷���һ֡���ݣ���֡���ݴ��з��͵�ַ������Ϊ�������Զ�Ӧ���ܣ�������Ϻ��Ͷ˻������������ģʽ��
 * ��ʱ���ն�����ӵ������Ժ�����ȻὫ���Ͷ˵ĵ�ַ�ͽ��ն˵ĵ�ַ���жԱȣ����һ�£����Խ��ն˵Ľ��յ�ַ�ͷ��Ͷ˵ķ��͵�ַҪһ�£�
 * ����Ϊ����ȷ�����ݣ�������ա����ն˻��������ͳ��Լ��Ľ��յ�ַ����ʱ���Ͷ�������յ����Ժ����Լ����õĽ��յ�ַ�Ƚ��Ƿ�һ�£�
 * ���һ�£�����Ϊ�˴η��ͳɹ���
**********************/
const uint8_t TX_ADDRESS[TX_ADR_WIDTH] = {0x11, 0x11, 0x10, 0x10, 0x01}; //���͵�ַ
const uint8_t RX_ADDRESS[RX_ADR_WIDTH] = {0x11, 0x11, 0x10, 0x10, 0x01}; //���͵�ַ

const uint8_t TX_ADDRESS1[TX_ADDR] = {0xAA, 0xBB, 0xCC, 0xDD, 0x01};
const uint8_t TX_ADDRESS2[TX_ADDR] = {0xDD, 0xAA, 0xCC, 0xDD, 0x01};
const uint8_t TX_ADDRESS3[TX_ADDR] = {0x02};

const uint8_t RX_ADDRESS1[TX_ADDR] = {0xAA, 0xBB, 0xCC, 0xDD, 0x01};
const uint8_t RX_ADDRESS2[TX_ADDR] = {0xDD, 0xAA, 0xCC, 0xDD, 0x01};
const uint8_t RX_ADDRESS3[TX_ADDR] = {0x02};

//SPI2 ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
uint8_t SPI2_ReadWriteByte(uint8_t TxData)
{
    uint8_t Rxdata;
    HAL_SPI_TransmitReceive(&hspi1,&TxData,&Rxdata,1, 1000);       
 	return Rxdata;          		    //�����յ�������		
}

//��ʼ��24L01��IO��
void NRF24L01_Init(void)
{
	NRF24L01_CE(0);  //ʹ��24L01
	NRF24L01_CSN(1); //SPIƬѡȡ��
}
//���24L01�Ƿ����
//����ֵ:0���ɹ�;1��ʧ��
uint8_t NRF24L01_Check(void)
{
	uint8_t buf[5] = {0XA5, 0XA5, 0XA5, 0XA5, 0XA5};
	uint8_t i;
	// SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_4);				 //spi�ٶ�Ϊ11.25Mhz��24L01�����SPIʱ��Ϊ10Mhz,�����һ��û��ϵ��
	NRF24L01_Write_Buf(NRF_WRITE_REG + TX_ADDR, buf, 5); //д��5���ֽڵĵ�ַ.
	NRF24L01_Read_Buf(TX_ADDR, buf, 5);					 //����д��ĵ�ַ
	for (i = 0; i < 5; i++)
		if (buf[i] != 0XA5)
			break;
	if (i != 5)
		return 1; //���24L01����
	return 0;	  //��⵽24L01
}
//SPIд�Ĵ���
//reg:ָ���Ĵ�����ַ
//value:д���ֵ
uint8_t NRF24L01_Write_Reg(uint8_t reg, uint8_t value)
{
	uint8_t status;
	NRF24L01_CSN(0);				  //ʹ��SPI����
	status = SPI2_ReadWriteByte(reg); //���ͼĴ�����
	SPI2_ReadWriteByte(value);		  //д��Ĵ�����ֵ
	NRF24L01_CSN(1);				  //��ֹSPI����
	return (status);				  //����״ֵ̬
}
//��ȡSPI�Ĵ���ֵ
//reg:Ҫ���ļĴ���
uint8_t NRF24L01_Read_Reg(uint8_t reg)
{
	uint8_t reg_val;
	NRF24L01_CSN(0);					//ʹ��SPI����
	SPI2_ReadWriteByte(reg);			//���ͼĴ�����
	reg_val = SPI2_ReadWriteByte(0XFF); //��ȡ�Ĵ�������
	NRF24L01_CSN(1);					//��ֹSPI����
	return (reg_val);					//����״ֵ̬
}
//��ָ��λ�ö���ָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ
uint8_t NRF24L01_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
	uint8_t status, uint8_t_ctr;
	NRF24L01_CSN(0);				  //ʹ��SPI����
	status = SPI2_ReadWriteByte(reg); //���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
	for (uint8_t_ctr = 0; uint8_t_ctr < len; uint8_t_ctr++)
		pBuf[uint8_t_ctr] = SPI2_ReadWriteByte(0XFF); //��������
	NRF24L01_CSN(1);								  //�ر�SPI����
	return status;									  //���ض�����״ֵ̬
}
//��ָ��λ��дָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ
uint8_t NRF24L01_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
	uint8_t status, uint8_t_ctr;
	NRF24L01_CSN(0);				  //ʹ��SPI����
	status = SPI2_ReadWriteByte(reg); //���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
	for (uint8_t_ctr = 0; uint8_t_ctr < len; uint8_t_ctr++)
		SPI2_ReadWriteByte(*pBuf++); //д������
	NRF24L01_CSN(1);				 //�ر�SPI����
	return status;					 //���ض�����״ֵ̬
}
//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:�������״��
uint8_t NRF24L01_TxPacket(uint8_t *txbuf)
{
	uint8_t sta;
	// SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_8); //spi�ٶ�Ϊ6.75Mhz��24L01�����SPIʱ��Ϊ10Mhz��
	NRF24L01_CE(0);
	NRF24L01_Write_Buf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH); //д���ݵ�TX BUF  32���ֽ�
	NRF24L01_CE(1);										//��������
	while (NRF24L01_IRQ != 0);
													 //�ȴ��������
	sta = NRF24L01_Read_Reg(STATUS);				 //��ȡ״̬�Ĵ�����ֵ
	NRF24L01_Write_Reg(NRF_WRITE_REG + STATUS, sta); //���TX_DS��MAX_RT�жϱ�־
	if (sta & MAX_TX)								 //�ﵽ����ط�����
	{
		NRF24L01_Write_Reg(FLUSH_TX, 0xff); //���TX FIFO�Ĵ���
		return MAX_TX;
	}
	if (sta & TX_OK) //�������
	{
		return TX_OK;
	}
	return 0xff; //����ԭ����ʧ��
}
//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:0��������ɣ��������������
uint8_t NRF24L01_RxPacket(uint8_t *rxbuf)
{
	uint8_t sta;
	sta = NRF24L01_Read_Reg(STATUS);				 //��ȡ״̬�Ĵ�����ֵ
	NRF24L01_Write_Reg(NRF_WRITE_REG + STATUS, sta); //���TX_DS��MAX_RT�жϱ�־
	if (sta & RX_OK)								 //���յ�����
	{
		NRF24L01_Read_Buf(RD_RX_PLOAD, rxbuf, RX_PLOAD_WIDTH); //��ȡ����
		NRF24L01_Write_Reg(FLUSH_RX, 0xff);					   //���RX FIFO�Ĵ���
		return 0;
	}
	return 1; //û�յ��κ�����
}
//�ú�����ʼ��NRF24L01��RXģʽ
//����RX��ַ,дRX���ݿ���,ѡ��RFƵ��,�����ʺ�LNA HCURR
//��CE��ߺ�,������RXģʽ,�����Խ���������
void NRF24L01_RX_Mode(void)
{
	NRF24L01_CE(0);
	NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, (uint8_t *)RX_ADDRESS, RX_ADR_WIDTH); //дRX�ڵ��ַ

	NRF24L01_Write_Reg(NRF_WRITE_REG + EN_AA, 0x01);			  //ʹ��ͨ��0���Զ�Ӧ��
	NRF24L01_Write_Reg(NRF_WRITE_REG + EN_RXADDR, 0x01);		  //ʹ��ͨ��0�Ľ��յ�ַ
	NRF24L01_Write_Reg(NRF_WRITE_REG + RF_CH, 40);				  //����RFͨ��Ƶ��
	NRF24L01_Write_Reg(NRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //ѡ��ͨ��0����Ч���ݿ���
	NRF24L01_Write_Reg(NRF_WRITE_REG + RF_SETUP, 0x0f);			  //����TX�������,0db����,2Mbps,���������濪��
	NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0f);			  //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ
	NRF24L01_CE(1);											  //CEΪ��,�������ģʽ
}

//�ú�����ʼ��NRF24L01��TXģʽ
//����TX��ַ,дTX���ݿ���,����RX�Զ�Ӧ��ĵ�ַ,���TX��������,ѡ��RFƵ��,�����ʺ�LNA HCURR
//PWR_UP,CRCʹ��
//��CE��ߺ�,������RXģʽ,�����Խ���������
//CEΪ�ߴ���10us,����������.
void NRF24L01_TX_Mode(void)
{
	NRF24L01_CE(0);
	NRF24L01_Write_Buf(NRF_WRITE_REG + TX_ADDR, (uint8_t *)TX_ADDRESS1, TX_ADR_WIDTH);	 //дTX�ڵ��ַ
	NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, (uint8_t *)RX_ADDRESS1, RX_ADR_WIDTH); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK

	NRF24L01_Write_Reg(NRF_WRITE_REG + EN_AA, 0x3f);	  //ʹ��ͨ��0���Զ�Ӧ��
	NRF24L01_Write_Reg(NRF_WRITE_REG + EN_RXADDR, 0x3f);  //ʹ��ͨ��0�Ľ��յ�ַ
	NRF24L01_Write_Reg(NRF_WRITE_REG + SETUP_RETR, 0xff); //�����Զ��ط����ʱ��:(0x1a 500us + 86us;����Զ��ط�����:10��)  (4000us + 86us 15��)
	NRF24L01_Write_Reg(NRF_WRITE_REG + RF_CH, 40);		  //����RFͨ��Ϊ40	2.4G + 40Mhz = 2.44G
	NRF24L01_Write_Reg(NRF_WRITE_REG + RF_SETUP, 0x0f);	  //����TX�������,0db����,2Mbps,���������濪��
	NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0e);	  //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
	NRF24L01_CE(1);									  //CEΪ��,10us����������
}

//���÷��������
void NRF_Set_Mode_Tx(uint8_t *addr)
{
	NRF24L01_CE(0);
	NRF24L01_Write_Buf(NRF_WRITE_REG + TX_ADDR, addr, TX_ADR_WIDTH);	 //дTX�ڵ��ַ
	NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, addr, RX_ADR_WIDTH);  //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK

	NRF24L01_Write_Reg(NRF_WRITE_REG + EN_AA, 0x3f);	  //ʹ��ͨ��0���Զ�Ӧ��
	NRF24L01_Write_Reg(NRF_WRITE_REG + EN_RXADDR, 0x3f);  //ʹ��ͨ��0�Ľ��յ�ַ
	NRF24L01_Write_Reg(NRF_WRITE_REG + SETUP_RETR, 0xff); //�����Զ��ط����ʱ��:(0x1a 500us + 86us;����Զ��ط�����:10��)  (4000us + 86us 15��)
	NRF24L01_Write_Reg(NRF_WRITE_REG + RF_CH, 40);		  //����RFͨ��Ϊ40
	NRF24L01_Write_Reg(NRF_WRITE_REG + RF_SETUP, 0x0f);	  //����TX�������,0db����,2Mbps,���������濪��
	NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0e);	  //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
	NRF24L01_CE(1);									  //CEΪ��,10us����������
}

//���÷��������
void NRF_Set_Mode_Rx(uint8_t *addr)
{
	NRF24L01_CE(0);
	NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, addr, RX_ADR_WIDTH); //дRX�ڵ��ַ

	NRF24L01_Write_Reg(NRF_WRITE_REG + EN_AA, 0x3f);			  //ʹ��ͨ��0���Զ�Ӧ��
	NRF24L01_Write_Reg(NRF_WRITE_REG + EN_RXADDR, 0x3f);		  //ʹ��ͨ��0�Ľ��յ�ַ
	NRF24L01_Write_Reg(NRF_WRITE_REG + RF_CH, 40);				  //����RFͨ��Ƶ��
	NRF24L01_Write_Reg(NRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //ѡ��ͨ��0����Ч���ݿ���
	NRF24L01_Write_Reg(NRF_WRITE_REG + RF_SETUP, 0x0f);			  //����TX�������,0db����,2Mbps,���������濪��
	NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0f);			  //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ
	NRF24L01_CE(1);									  //CEΪ��,10us����������
}


//����NRF24L01����ģʽ
void NRF24L01_Set_Mode(__NRFRESULT _mode, uint8_t *addr)
{
	
	if(_mode == NRF_MODE_TX){
		
	}
	else if(_mode == NRF_MODE_RX){
		
	}
	
}

//NRF24L01����������
//���ݳ����Զ����Ҳ��ܳ���32�ֽ�
__NRFRESULT NRF24L01_TxData(uint8_t *_txvar, uint8_t len)
{
	uint8_t sta;
	NRF24L01_CE(0);
	if(len < TX_PLOAD_MAX_WIDTH){		//С��������ֽ�
		NRF24L01_Write_Buf(WR_TX_PLOAD, _txvar, len); //д���ݵ�TX BUF  32���ֽ�
		NRF24L01_CE(1);										//��������
		while (NRF24L01_IRQ != 0);		//�ȴ��������
		sta = NRF24L01_Read_Reg(STATUS);				 //��ȡ״̬�Ĵ�����ֵ
		NRF24L01_Write_Reg(NRF_WRITE_REG + STATUS, sta); //���TX_DS��MAX_RT�жϱ�־
		if (sta & MAX_TX){							 	//�ﵽ����ط�����
			NRF24L01_Write_Reg(FLUSH_TX, 0xff); //���TX FIFO�Ĵ���
			return (__NRFRESULT)MAX_TX;
		}
		if (sta & TX_OK){ //�������
			return NRF_OK;
		}		
	}
	return NRF_ERROR; //����ԭ����ʧ��		
}

