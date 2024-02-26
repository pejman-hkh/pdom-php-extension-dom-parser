<?php

$data = file_get_contents('fight-club.html');

function findAttr( $attrs = [], $tags ) {
    $ret = [];

    foreach( $tags as $tag ) {
        $f = true;
        foreach( $attrs as $attr => $value ) {
            if( $attr == 'class' ) {
                $classes = @$tag['attrs']?explode(" ", @$tag['attrs']['class']):[];
                if( ! in_array($value, $classes ) ) 
                    $f = false;
            } else {	
                $g = $attr == 'tag' ? @$tag['tag'] : @$tag['attrs'][$attr];
                if( $g !== $value  )
                    $f = false;
            }

        }

        if( $f )
            $ret[] = $tag;

        if( count((array)@$tag['childrens']) > 0 ) {
            $found = findAttr( $attrs, $tag['childrens'] );
            foreach( $found as $a ) {
                $ret[] = $a;
            }
        }
    }

    return $ret;
}

$s = microtime(1);

$dom = pdom( $data );
//print_r( $dom );

file_put_contents( 'a.json', json_encode( $dom ) );
// $find = findAttr( ['class' => 'ipc-image'], $dom['document'] );
// print_r( $find[0]['attrs']['src']."\n" );

echo (microtime(1) - $s) * 1000;

echo "\n";

$s = microtime(1);

$doc = new DOMDocument();
@$doc->loadHTML($data);
echo (microtime(1) - $s) * 1000;

echo "\n";