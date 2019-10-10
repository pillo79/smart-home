#include "logicio.h"
#include "logicdevice.h"

LogicIO::LogicIO(const char* name, LogicDevice *dev)
	: m_name(name)
	, m_dev(dev)
{

}

LogicIO::~LogicIO()
{

}



BitInput::BitInput(const char *name, LogicDevice *dev, int bitAddr)
	: LogicIO(name, dev)
{
	m_bitAddr = bitAddr;
}

int BitInput::getValue() const
{
	return m_dev->getDigInput(m_bitAddr);
}



BitOutput::BitOutput(const char *name, LogicDevice *dev, int bitAddr)
	: LogicIO(name, dev)
{
	m_bitAddr = bitAddr;
}

int BitOutput::getValue() const
{
	return m_dev->getDigOutput(m_bitAddr);
}

void BitOutput::setValue(int value)
{
	m_dev->setDigOutput(m_bitAddr, !!value);
}



WordInput::WordInput(const char *name, LogicDevice *dev, int wordAddr)
	: LogicIO(name, dev)
{
	m_wordAddr = wordAddr;
}

int WordInput::getValue() const
{
	return m_dev->getInputVal(m_wordAddr);
}



WordOutput::WordOutput(const char *name, LogicDevice *dev, int wordAddr)
	: LogicIO(name, dev)
{
	m_wordAddr = wordAddr;
}

int WordOutput::getValue() const
{
	return m_dev->getOutputVal(m_wordAddr);
}

void WordOutput::setValue(int value)
{
	m_dev->setOutputVal(m_wordAddr, value);
}



FloatInput::FloatInput(const char *name, LogicDevice *dev, int wordAddr)
	: LogicIO(name, dev)
{
	m_wordAddr = wordAddr;
}

int FloatInput::getValue() const
{
	union {
		unsigned char bytes[4];
		unsigned short words[2];
		float f;
	} v;

	v.words[0] = m_dev->getInputVal(m_wordAddr);
	v.words[1] = m_dev->getInputVal(m_wordAddr+1);

	return (int) v.f;
}
