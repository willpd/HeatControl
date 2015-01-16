<?php
ini_set('display_errors', 'On');
error_reporting(E_ALL);
?>
<!DOCTYPE html> 
<html>
<head>
	<title>Office Heat Monitor</title>
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<link rel="stylesheet" href="http://code.jquery.com/mobile/1.4.5/jquery.mobile-1.4.5.min.css" />
	<script src="http://code.jquery.com/jquery-1.11.1.min.js"></script>
	<script src="http://code.jquery.com/mobile/1.4.5/jquery.mobile-1.4.5.min.js"></script>
	<style>
	.ui-header h<x> {
		text-overflow: initial;
	}
	</style>
	<script>
	var heatState;
	$(document).ready(function() 
	{
		loadData(true);
		
		$("#heatSwitch").change(function() 
		{
			alert(heatState);
			alert($("#heatSwitch").is(":checked"));
			if ($("#heatSwitch").is(":checked") && (heatState == false))
			{
				// Make a call to the power control on the board
				$.ajax({
				dataType: "json",
				url: "http://192.168.1.32/powerControl?params=1",
				success: function (data) {
					loadData(false);
				}
				});			
			}
			else if (!$("#heatSwitch").is(":checked") && (heatState == true))
			{
				// Make a call to the power control on the board
				$.ajax({
				dataType: "json",
				url: "http://192.168.1.32/powerControl?params=0",
				success: function (data) {
					loadData(false);
				}
				});			
			
			}
		});
		
	
	});
	
	function loadData(state)
	{
		// Call the temperature service
		$.ajax({
			dataType: "json",
			url: "http://192.168.1.32/temperature",
			success: function (data) {
				var tempVal = data["temperature"];
				tempVal += "&deg; F";
				$("#currTemp").html(tempVal);
			}
			});
		if (state)
		{
			// Call the state service
			$.ajax({
				dataType: "json",
				url: "http://192.168.1.32/state",
				success: function (data) {
					var stateVal = data["state"];
					if (stateVal == 1)
					{
						stateVal = "Power is On";
						$("#heatSwitch").prop("checked", true).flipswitch("refresh");
						heatState = true;
					}
					else
					{
						stateVal = "Power is Off";
						$("#heatSwitch").prop("checked",false).flipswitch("refresh");
						heatState = false;
					}
					$("#currState").html(stateVal);
				}
				});
		}
		// Call the working service
		$.ajax({
			dataType: "json",
			url: "http://192.168.1.32/working",
			success: function (data) {
				var workingVal = data["working"];
				if (workingVal == 1)
					workingVal = "It is inside working hours";
				else
					workingVal = "It is outside working hours";
				$("#currWorkTime").html(workingVal);
			}
			});		
			
	}
	</script>
</head>

<body>
	<!-- Display the current state, temp, & date -->
	<div data-role-"page">
		<div data-role="header">
			<h1>Office Heat Monitor</h1>
		</div>
		
		<div role="main" class="ui-content">
			<div id="CurrentTempContainer">
				The Current Temp is: <div id="currTemp">Scanning...</div> 
			</div>
			<div id="CurrentStateContainer">
				<div id="currState">Scanning...</div> 
			</div>
			<div id="CurrentWorkingContainer">
				<div id="currWorkTime">Scanning...</div> 
			</div>
			<form>
			    <label for="flip-checkbox-1">Turn Heat On/Off:</label>
			    <input type="checkbox" data-role="flipswitch" name="heatSwitch" id="heatSwitch">
			</form>
		</div>
	</div>
</body>


</html>