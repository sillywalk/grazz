#include "mupdf/fitz.h"
#include "mupdf/pdf.h"

#include <string.h>
#include <stdlib.h>

#include "encodings.h"
#include "glyphlist.h"

#define FROM_UNICODE(ENC) \
{ \
	int l = 0; \
	int r = nelem(ENC##_from_unicode) - 1; \
	if (u < 128) \
		return u; \
	while (l <= r) \
	{ \
		int m = (l + r) >> 1; \
		if (u < ENC##_from_unicode[m].u) \
			r = m - 1; \
		else if (u > ENC##_from_unicode[m].u) \
			l = m + 1; \
		else \
			return ENC##_from_unicode[m].c; \
	} \
	return -1; \
}

int fz_iso8859_1_from_unicode(int u) FROM_UNICODE(iso8859_1)
int fz_iso8859_7_from_unicode(int u) FROM_UNICODE(iso8859_7)
int fz_koi8u_from_unicode(int u) FROM_UNICODE(koi8u)
int fz_windows_1250_from_unicode(int u) FROM_UNICODE(windows_1250)
int fz_windows_1251_from_unicode(int u) FROM_UNICODE(windows_1251)
int fz_windows_1252_from_unicode(int u) FROM_UNICODE(windows_1252)

int
fz_unicode_from_glyph_name(const char *name)
{
	char buf[64];
	char *p;
	int l = 0;
	int r = nelem(agl_name_list) - 1;
	int code = 0;

	fz_strlcpy(buf, name, sizeof buf);

	/* kill anything after first period and underscore */
	p = strchr(buf, '.');
	if (p) p[0] = 0;
	p = strchr(buf, '_');
	if (p) p[0] = 0;

	while (l <= r)
	{
		int m = (l + r) >> 1;
		int c = strcmp(buf, agl_name_list[m]);
		if (c < 0)
			r = m - 1;
		else if (c > 0)
			l = m + 1;
		else
			return agl_code_list[m];
	}

	if (buf[0] == 'u' && buf[1] == 'n' && buf[2] == 'i')
		code = strtol(buf + 3, NULL, 16);
	else if (buf[0] == 'u')
		code = strtol(buf + 1, NULL, 16);
	else if (buf[0] == 'a' && buf[1] != 0 && buf[2] != 0)
		code = strtol(buf + 1, NULL, 10);

	return (code > 0 && code <= 0x10ffff) ? code : FZ_REPLACEMENT_CHARACTER;
}

static const char *empty_dup_list[] = { 0 };

const char **
fz_duplicate_glyph_names_from_unicode(int ucs)
{
	int l = 0;
	int r = nelem(agl_dup_offsets) / 2 - 1;
	while (l <= r)
	{
		int m = (l + r) >> 1;
		if (ucs < agl_dup_offsets[m << 1])
			r = m - 1;
		else if (ucs > agl_dup_offsets[m << 1])
			l = m + 1;
		else
			return agl_dup_names + agl_dup_offsets[(m << 1) + 1];
	}
	return empty_dup_list;
}
