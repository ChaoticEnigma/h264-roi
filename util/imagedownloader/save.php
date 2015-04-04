
<?php

//var $origin = $_SERVER['HTTP_REFERER'];
//var $proto = parse_url($origin, PHP_URL_SCHEME);
//var $host = parse_url($origin, PHP_URL_HOST);
//header('Access-Control-Allow-Origin: '.$proto.'://'.$host);

header('Access-Control-Allow-Origin: https://e621.net');

file_put_contents($_SERVER["DOCUMENT_ROOT"]."/data.txt", $HTTP_RAW_POST_DATA."\n", FILE_APPEND);

?>

<!DOCTYPE HTML>
<html>
<body>
    <p>Not Here!</p>
</body>
</html>