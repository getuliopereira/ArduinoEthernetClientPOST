<?php
   	include("connect.php");
   	
   	$link=Connection();

	$temp1=$_POST["t"];
	$hum1=$_POST["u"];

	$query = "INSERT INTO `tempLog`(`temperatura`,`umidade`) VALUES ($temp1,$hum1)"; 
   	
   	mysql_query($query,$link);
	mysql_close($link);

   	header("Location: index.php");
?>
