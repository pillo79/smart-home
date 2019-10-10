#ifndef __LOGICIO_H__
#define __LOGICIO_H__

#include "global.h"

class LogicDevice;

class LogicIO {
	public:
		LogicIO(const char* name, LogicDevice *dev);
		virtual ~LogicIO();

		virtual const char* name() const { return m_name; };
		virtual int  getValue() const { return -ENXIO; }
		virtual void setValue(int value) { UNUSED(value); }
	protected:
		LogicDevice *m_dev;
		const char* m_name;
};

class BitInput : public LogicIO {
		int m_bitAddr;
	public:
		BitInput(const char *name, LogicDevice *dev, int bitAddr);
		virtual ~BitInput() { }

		virtual int getValue() const;
};

class BitOutput : public LogicIO {
		int m_bitAddr;
	public:
		BitOutput(const char *name, LogicDevice *dev, int bitAddr);
		virtual ~BitOutput() { }

		virtual int  getValue() const;
		virtual void setValue(int value);
};

class WordInput : public LogicIO {
		int m_wordAddr;
	public:
		WordInput(const char *name, LogicDevice *dev, int wordAddr);
		virtual ~WordInput() { }

		virtual int getValue() const;
};

class WordOutput : public LogicIO {
		int m_wordAddr;
	public:
		WordOutput(const char *name, LogicDevice *dev, int wordAddr);
		virtual ~WordOutput() { }

		virtual int  getValue() const;
		virtual void setValue(int value);
};

class FloatInput : public LogicIO {
		int m_wordAddr;
	public:
		FloatInput(const char *name, LogicDevice *dev, int wordAddr);
		virtual ~FloatInput() { }

		virtual int getValue() const;
};

#endif /* __LOGICIO_H__ */
