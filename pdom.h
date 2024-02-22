#ifndef DOM_PARSER
#define DOM_PARSER

#include <stdint.h>
#include "php.h"


typedef struct
{
	zval *childrens;
	zval *attrs;
	char *tag;
	size_t isEnd;
	char *content;
	int eq;
	struct pdom_Tag *next;
	struct pdom_Tag *prev;
	struct pdom_Tag *parent;
} pdom_Tag;

typedef struct
{
	pdom_Tag **tags;
	char *html;
	int i;
	pdom_Tag *current;
	size_t isXml;
} pdom_Parser;

size_t pdom_isEqual(pdom_Parser *p, char *text);
zval *pdom_parseAttr(pdom_Parser *p);
size_t pdom_parseTag(pdom_Parser *p, pdom_Tag *tag);
char *pdom_getContentUntil( pdom_Parser *p, char until, char *str, char first );
size_t pdom_parseContents(pdom_Parser *p, char first, pdom_Tag *tag);
size_t pdom_parseComment(pdom_Parser *p, pdom_Tag *tag);
char *pdom_parseScriptInner(pdom_Parser *p);
char *pdom_parseCData(pdom_Parser *p);
size_t pdom_next1(pdom_Parser *p, pdom_Tag *tag);
size_t pdom_next(pdom_Parser *p, pdom_Tag *tag);
size_t pdom_isEqual(pdom_Parser *p, char *text);
int pdom_checkHasEndTag(pdom_Tag *tag);
int pdom_getTag(pdom_Parser *p, pdom_Tag *tag);
zval *pdom_parse(pdom_Parser *p, pdom_Tag *parent);


char *hasNoEndTags[17] = {"comment", "php", "empty", "!DOCTYPE", "area", "base", "col", "embed", "param", "source", "track", "meta", "link", "br", "input", "hr", "img"};

zval *pdom_parseAttr(pdom_Parser *p)
{
	zval *attrs = (zval*)malloc(sizeof(zval));
	array_init(attrs);

	char *attr = malloc(sizeof(char));
	char *nowAttr = malloc(sizeof(char));
	char c1;
	int i = 0;
	size_t size = 0, len = 0;
	while (1)
	{
		char c1 = *p->html++;
		if(  c1 == '\0' && c1 != '0' )
			break;
		

		if (c1 == ' ')
		{
			len = 0, size = 0;
			attr = malloc(sizeof(char));
			continue;
		}

		char t = 0;
		if (c1 == '=')
		{
			attr[len] = '\0';
			nowAttr = malloc(strlen(attr));
			nowAttr = attr;

			attr = malloc(sizeof(char));
			len = 0, size = 0;
			char g = *p->html;
			if (g == '"' || g == '\'')
			{
				t = g;
				p->html++;
			}

			char *value = malloc(sizeof(char));
			char c2;
			size_t size1 = 0, len1 = 0;
			while (1)
			{

				char c2 = *p->html++;
				if(  c2 == '\0' && c2 != '0' )
					break;
		

				if (!t && c2 == ' ')
					break;
				

				if (!t && c2 == '>')
				{
					p->html--;
					break;
				}

				if (c2 == t)
					break;

				if (len1 + 1 >= size1)
				{
					size1 = size1 * 2 + 1;
					value = realloc(value, sizeof(char) * size1);
				}

				value[len1++] = c2;
			}
			value[len1] = '\0';

			add_assoc_string(attrs, nowAttr, value);
			len = 0, size = 0;
			attr = malloc(sizeof(char));
		}

		if (!t && c1 == '=')
			continue;

		if (c1 == '>')
			break;
		
		if (len + 1 >= size)
		{
			size = size * 2 + 1;
			attr = realloc(attr, sizeof(char) * size);
		}

		attr[len++] = c1;
	}

	return attrs;
}

size_t pdom_parseTag(pdom_Parser *p, pdom_Tag *tag)
{
	if (pdom_isEqual(p, "![CDATA["))
	{
		p->html += 8;
		tag->tag = "cdata";
		return 1;
	}

	if ( *(p->html + 1) == '/')
		p->html++;

	char *name = malloc(sizeof(char));

	zval *attrs = 0;
	size_t size = 0, len = 0;
	while (1)
	{
		char c1 = *p->html++;
		if(  c1 == '\0' && c1 != '0' )
			break;
		
		if (c1 == '>')
			break;
		

		if (c1 == ' ')
		{
			attrs = pdom_parseAttr(p);
			break;
		}

		if (len + 1 >= size)
		{
			size = size * 2 + 1;
			name = realloc(name, sizeof(char) * size);
		}

		name[len++] = c1;
	}

	name[len] = '\0';

	tag->tag = name;
	if( attrs )
		tag->attrs = attrs;
	tag->isEnd = 0;

	if (*name == '/')
	{
		tag->isEnd = 1;
		tag->tag = name + 1;
	}
	if (name[len-1] == '/')
	{
		name[strlen(name) - 1] = '\0';
		tag->tag = name;
	}
	return 1;
}

char *pdom_getContentUntil( pdom_Parser *p, char until, char *str, char first ) {
	char *content = malloc(sizeof(char) * 2);
	size_t size = 0, len = 0;

	if (first)
	{
		content[0] = first;
		len++;
	}

	char c1;
	while (1)
	{

		char c1 = *p->html++;
		if(  c1 == '\0' && c1 != '0' )
			break;
		

		if ( str != 0 ) {

			if( c1 == until && pdom_isEqual(p, str) ) {
				break;
			}
		}
		else if ( c1 == until )
			break;

		if (len + 1 >= size)
		{
			size = size * 2 + 1;
			content = realloc(content, sizeof(char) * size);
		}

		content[len++] = c1;
	}
	content[len] = '\0';
	return content;
}

size_t pdom_parseContents(pdom_Parser *p, char first, pdom_Tag *tag)
{
	p->html--;

	char *content = pdom_getContentUntil(p, '<', 0, first);
	
	p->html--;

	tag->tag = "empty";
	tag->content = content;
	return 1;
}

size_t pdom_parseComment(pdom_Parser *p, pdom_Tag *tag)
{
	p->html += 3;

	char *content = pdom_getContentUntil(p,'-', "->", 0);

	p->html += 2;

	tag->tag = "comment";
	tag->content = content;
	return 1;
}

char *pdom_parseScriptInner(pdom_Parser *p)
{
	char *content = pdom_getContentUntil(p, '<', "/script", 0);
	p->html += 8;
	return content;
}

char *pdom_parseCData(pdom_Parser *p)
{
	char *content = pdom_getContentUntil(p,']', "]>", 0);
	p->html += 2;
	return content;
}

size_t pdom_next1(pdom_Parser *p, pdom_Tag *tag)
{
	char c = *p->html++;
	
	if(  c == '\0' && c != '0' )
		return 0;
		
	if (c == '<')
	{
		if (pdom_isEqual(p, "!--"))
		{
			return pdom_parseComment(p, tag);
		}

		if (*p->html == ' ')
		{
			p->html++;

			return pdom_parseContents(p, '<', tag);
		}

		return pdom_parseTag(p, tag);
	}
	else
	{
		return pdom_parseContents(p, 0, tag);
	}
}

size_t pdom_next(pdom_Parser *p, pdom_Tag *tag)
{
	size_t ret = pdom_next1(p, tag);
	if (ret)
		p->current = tag;
	return ret;
}

size_t pdom_isEqual(pdom_Parser *p, char *text)
{

	char *html = p->html;
	char c;
	while (c = *text++)
	{
		char h = *html++;

		if (h == EOF)
		{
			return 0;
		}

		if (h != c)
			return 0;
	}

	return 1;
}

int pdom_checkHasEndTag(pdom_Tag *tag)
{
	for (int i = 0; i < 17; i++)
	{
		if (strcmp(tag->tag, hasNoEndTags[i]) == 0)
			return 1;
	}
	return 0;
}

int pdom_getTag(pdom_Parser *p, pdom_Tag *tag)
{

	int ret = pdom_next(p, tag);
	if ( ! ret )
		return 0;
	

	if (strcmp(tag->tag, "cdata") == 0)
	{
		tag->content = pdom_parseCData(p);
		return 1;
	}

	char *substr = malloc(sizeof(char)*5);
	for( int i = 0; i < 4; i++) {
		substr[i] = tag->tag[i];
	}
	substr[4] = '\0';

	if (strcmp(substr, "?xml") == 0)
	{
		p->isXml = 1;
		return 1;
	}

	if (p->isXml)
		hasNoEndTags[11] = "";


	if (pdom_checkHasEndTag(tag))
		return 1;

	if (tag->isEnd)
		return 1;

	if ( strcmp(tag->tag, "script") ==0 )
	{
		char *content = pdom_parseScriptInner(p);
		tag->content = content;
	}
	else
	{

		tag->childrens = pdom_parse(p, tag);
	}

	if (strcmp(tag->tag, p->current->tag) == 0)
	{
		return 1;
	}

	pdom_Tag *etag = malloc(sizeof(pdom_Tag));

	while (pdom_next(p, etag))
	{
		if (strcmp(tag->tag, etag->tag) == 0)
			break;
	}

	return 1;
}

zval *pdom_parse(pdom_Parser *p, pdom_Tag *parent)
{

	zval *tags = (zval*)malloc(sizeof(zval));
	array_init(tags);

	pdom_Tag *stag = malloc(sizeof(pdom_Tag));
	int eq = 0;
	int it = 0;

	while (1)
	{
		if( p->html[p->i] == '\0' ) {
			break;
		}

		pdom_Tag *tag = malloc(sizeof(pdom_Tag));

		tag->isEnd = 0;
		tag->childrens = 0;
		tag->attrs = 0;
		tag->content = "";
		tag->eq = 0;
		tag->next = 0;
		tag->prev = 0;
		tag->parent = 0;
		int st = pdom_getTag(p, tag);
	

		if ( !st )
			break;


		if (tag->isEnd && strcmp(parent->tag, tag->tag) == 0)
			break;
		
		if (!tag->isEnd)
		{
			
			// tag->eq = eq++;
			// tag->prev = stag;
			// tag->parent = parent;
			// stag->next = tag;

			zval *arrTag = (zval*)malloc(sizeof(zval));
			array_init(arrTag);
			add_assoc_string(arrTag, "tag", tag->tag);
			
			if( tag->content )
				add_assoc_string(arrTag, "content", tag->content);

			if( tag->attrs )
				add_assoc_zval(arrTag, "attrs", tag->attrs);
			if( tag->childrens )
				add_assoc_zval(arrTag, "childrens", tag->childrens);

			add_next_index_zval(tags, arrTag);

		}

		stag = tag;
	}

	return tags;
}

#endif
