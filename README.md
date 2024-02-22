# PDOM PHP extension DOM parser
PHP Extension for HTML DOM Parsing

This `php` extension is 3 times faster than `DOMDocument` and 5 times faster than `simple_html_dom` and 5 times faster than my `php dom parser`.
https://github.com/pejman-hkh/dom-parser

I'm working on it to add a query string parser and...

# Install
First, clone it, and then:
```
phpize
./configure --enable-pdom
make
sudo make install
```
At the end, add the `.ini` file in the `ini` path with this content
```
extension=pdom.so
```
This is just a parser and returns a PHP array with DOM details.
To find it in this array, I have written a tiny PHP function that exists in `test.php`