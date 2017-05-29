<!DOCTYPE html>
<html lang="en">
<head>
	<title>Medical Record</title>
</head>
<body>
<?php
// Refresh
$page = $_SERVER['PHP_SELF'];
$sec = "5";
header("Refresh: $sec; url=$page");

$servername = "localhost";
$username = "root";
$password = "";
$dbname = "MedicalRecord";
// Create connection
$conn = new mysqli($servername, $username, $password, $dbname) or die("Connection failed: " . $conn->connect_error);

$table_name = "Heartbeats";
$column_names = "heartbeat, diagnosis";

if ( $_SERVER['REQUEST_METHOD'] == 'POST' ) {
	$new_data = file_get_contents('php://input');
	//echo "$new_data\n";

	// Insert new data
	$sql = sprintf("INSERT INTO %s(%s) VALUES(NULL,'%s')", $table_name, $column_names, $new_data);
	if ($conn->query($sql) !== TRUE) {
		echo "<h1>SQL ERROR</h1>" . $conn->error;
	}
}

// Display data
$sql = sprintf("SELECT %s FROM %s", $column_names, $table_name);
$result = $conn->query($sql);
if ($result->num_rows > 0) {
	?>
	<h1>Medical Record</h1>
	<table>
	<tr>
		<th> heartbeat </th>
		<th> diagnosis </th>
	</tr>
	<?php
	while($row = $result->fetch_assoc()) {
		echo "<tr>\n";
        	echo "<td>" . $row["heartbeat"]. "</td><td>" . $row["diagnosis"] . "</td>\n";
		echo "</tr>";
	}
	echo "</table>";
} 
else {
	echo "0 results";
}

// Close connection
$conn->close();
?>
</body>
</html> 
