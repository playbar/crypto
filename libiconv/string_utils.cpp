#include <assert.h>
#include <stdlib.h>
#include "string_utils.h"
#include "string_comp.h"


#if 0
#include "unicode/ucnv.h"

char * convertString(const char * fromCode, const char * toCode, const char * source)
{
	const char * toConverterName = toCode;
	const char * fromConverterName = fromCode;

	int32_t sourceLength = strlen(source);
	if (!strncmp(fromCode, "UTF-16LE", 8))
	{
		sourceLength = gg_wcslen((unsigned short *)source) * 2;
	}
	int32_t targetCapacity = sourceLength * 4 + 2;

	char * target = new char[targetCapacity];
	memset(target, 0, targetCapacity);

	UErrorCode pErrorCode = U_ZERO_ERROR;

	int32_t ret = ucnv_convert(toConverterName, fromConverterName, target, targetCapacity, source, sourceLength, &pErrorCode);

    if(U_FAILURE(pErrorCode))
    {
		memcpy(target, source, sourceLength);
		ret = sourceLength;
    }
    else
    {
    	memset(target + ret, 0, targetCapacity - ret);
    }

	return target;
}
#else
#include "iconv.h"

char * convertString(const char * fromCode, const char * toCode, const char * source, int len)
{
	size_t retlen = 0;
	size_t inbytesleft = len;
    if( strncmp(fromCode, "UTF-16LE", 8) == 0)
    {
        inbytesleft *= 2;
    }
	if( inbytesleft == 0) {
		inbytesleft = strlen(source);
		if (!strncmp(fromCode, "UTF-16LE", 8)) {
			inbytesleft = gg_wcslen((unsigned short *) source) * 2;
		}
	}
	size_t outbytesleft = inbytesleft * 4 + 2;

	char * tmp = new char[outbytesleft];
	memset(tmp, 0, outbytesleft);

	if (inbytesleft > 0)
	{
		char * inbuf = (char *)source;
		char * outbuf = tmp;

		retlen = outbytesleft;
		iconv_t cd = iconv_open(toCode, fromCode);
		size_t errorNo = iconv(cd, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
		retlen -= outbytesleft;

		if (errorNo == -1)
		{
			memcpy(tmp, source, inbytesleft);
			retlen = inbytesleft;
		}
		else
		{
			memset(tmp+retlen, 0, outbytesleft);
		}

		iconv_close(cd);
	}
	return tmp;
}
#endif
unsigned short* gbkToUnicode(const char* gbk, int len)
{
	return (unsigned short*)convertString("GBK","UTF-16LE",gbk, len);
}

//unsigned short* gbkToUnicode(const char* gbk, int & len)
//{
//	char * tmp = new char[len + 1];
//	memcpy(tmp, gbk, len);
//	tmp[len]=0;
//
//	unsigned short* unicode =  gbkToUnicode(tmp);
//	len = gg_wcslen((unsigned short *)unicode);
//
//	delete[] tmp;
//	return unicode;
//}

char* gbkToUtf8(const char* gbk, int len)
{
	return convertString("GBK","UTF-8",gbk, len);
}

char* unicodeToGbk(const unsigned short* unicode, int len)
{
	return convertString("UTF-16LE","GBK",(const char*)unicode, len);
}

char* unicodeToUtf8(const unsigned short* unicode, int len)
{
	return convertString("UTF-16LE","UTF-8",(const char*)unicode, len);
}

unsigned short* utf8ToUnicode(const char* utf8, int len)
{
	return (unsigned short*)convertString("UTF-8","UTF-16LE",utf8, len);
}

char* utf8ToGbk(const char* utf8, int len)
{
	return convertString("UTF-8","GBK",utf8, len);
}

