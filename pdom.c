#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_pdom.h"
#include "pdom.h"

static zend_function_entry pdom_functions[] = {
    PHP_FE(pdom, NULL)
    {NULL, NULL, NULL}
};

zend_module_entry pdom_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_PDOM_EXTNAME,
    pdom_functions,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
#if ZEND_MODULE_API_NO >= 20010901
    PHP_PDOM_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_PDOM
ZEND_GET_MODULE(pdom)
#endif

PHP_FUNCTION(pdom)
{
	zend_string *html;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(html)
	ZEND_PARSE_PARAMETERS_END();


 	pdom_Parser *p = malloc(sizeof(pdom_Parser));
	p->html = html->val;
	p->i = 0;
	pdom_Tag *document = malloc(sizeof(pdom_Tag));
	document->tag = "document";
	p->current = document;
	document->childrens = pdom_parse(p, document);

	array_init(return_value);
	add_assoc_zval(return_value, "document", document->childrens );

	free(document);
	free(p);

}