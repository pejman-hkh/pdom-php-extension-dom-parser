PHP_ARG_ENABLE(pdom, whether to enable PDOM Parser,
[ --enable-pdom   Enable PDOM Parser])

if test "$PHP_PDOM" = "yes"; then
  AC_DEFINE(HAVE_PDOM, 1, [PHP HTML DOM Parser])
  PHP_NEW_EXTENSION(pdom, pdom.c, $ext_shared)
fi