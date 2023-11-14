<?php

if(isset($_GET["serial"])) {
   $serial = $_GET["serial"]; // get serial value from HTTP GET

   $servername = "localhost";
   $username = "ESP32";
   $password = "esp32io.com";
   $database_name = "db_esp32";

   // Create MySQL connection fom PHP to MySQL server
   $connection = new mysqli($servername, $username, $password, $database_name);
   // Check connection
   if ($connection->connect_error) {
      die("MySQL connection failed: " . $connection->connect_error);
   }

   $sql = "SELECT FROM tbl_serial WHERE serial = $serial";

   if ($connection->query($sql) === TRUE) {
      echo "New record created successfully";
   } else {
      echo "Error: " . $sql . " => " . $connection->error;
   }

   $connection->close();
} else {
   echo "serial is not set in the HTTP request";
}
?>
