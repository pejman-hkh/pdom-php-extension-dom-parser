#ifndef PHP_PDOM_H
#define PHP_PDOM_H 1

#define PHP_PDOM_VERSION "1.0"
#define PHP_PDOM_EXTNAME "pdom"

PHP_FUNCTION(pdom);

extern zend_module_entry pdom_module_entry;
#define phpext_pdom_ptr &pdom_module_entry

#endif