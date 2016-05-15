<?php
$startSH_Time = microtime(true);
$curl_sh = curl_init('https://infoconnect1.highwayinfo.govt.nz/ic/jbi/TREIS/REST/FeedService/');

curl_setopt($curl_sh, CURLOPT_HTTPHEADER, array(
    'username: mjwynyard', 'password: Copper2004'
        )
);
curl_setopt($curl_sh, CURLOPT_RETURNTRANSFER, true);
$responseSH = curl_exec($curl_sh);

file_put_contents("roadSH.xml", $responseSH);
$endSH_Time = microtime(true);

echo "xml downloaded: " .($endSH_Time - $startSH_Time) ." seconds<br>";


#
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
# The code above gets the xml (using the username and password details )
#  as a string then saves it as an xml file called roadClosures.xml
#

$xml = simplexml_load_file("roadSH.xml"); //or die("Error: Cannot create object");
$result = $xml->xpath("//tns:roadEvent");
$count = sizeof($result);
$warning = $xml->xpath("//tns:northIslandWarning/tns:message");
$ids = $xml->xpath("//tns:eventId");
$locations = $xml->xpath("//tns:locationArea");
$descriptions = $xml->xpath("//tns:eventComments");
$endDates = $xml->xpath("//tns:endDate");
$planned = $xml->xpath("//tns:planned");
$alternativeRoute = $xml->xpath("//tns:alternativeRoute");
$eventTypes = $xml->xpath("//tns:eventType");
$expectedResoultion = $xml->xpath("//tns:expectedResolution");
$impact = $xml->xpath("//tns:impact");
$locationAreas = $xml->xpath("//tns:locationArea");
$restrictions = $xml->xpath("//tns:restrictions");
$startDates = $xml->xpath("//tns:startDate");
$status = $xml->xpath("//tns:status");
$eventCreated = $xml->xpath("//tns:eventCreated");
$eventModified = $xml->xpath("//tns:eventModified");

$testPoints = $xml->xpath("//tns:wktGeometry");
$testPointsSize = sizeof($testPoints);

$exploded = array();
$finalPoints = array();

echo $warning[0];

for ($i = 0; $i < $testPointsSize; $i++) {
    $exploded[$i] = explode(";", $testPoints[$i]);
    $points[$i] = $exploded[$i][1];

    if (substr($points[$i], 0, 1) === "P") {
        $tempP = explode("(", $points[$i]);
        $tempP2 = substr($tempP[1], 0, -1);
        $finalPoints[$i] = $tempP2;
    } else {
        $tempM = explode("((", $points[$i]);
        $tempM2 = substr($tempM[1], 0, -2);
        $tempM3 = str_replace(",", "", $tempM2);
        $finalPoints[$i] = $tempM3;
    }
}

echo "Number of road events: " . "$count" . "<br>";
echo "Number of ids: " . sizeof($ids) . "<br>";

$tempStructArray = array();
$toBeEncoded = array();

//function convertDates($x) {
//    $tempDateArr = explode("T", $x);
//    $dateConv = $tempDateArr[0];
//    $dateArr = explode("-", $dateConv);
//    $rawTime = $tempDateArr[1];
//    $finalTime = substr($rawTime, 0, 5);
//    $dateFinal = "Date: " . $dateArr[2] . "-" . $dateArr[1] . "-" . $dateArr[0] . " Time(24hr format): " . $finalTime;
//    if ($dateFinal === "Date: -- Time(24hr format): ") {
//        $dateFinal = "No end date set";
//        return $dateFinal;
//    } else {
//        return $dateFinal;
//    }
//}


#############################################################################



$sizeT = sizeof($finalPoints);
$des = array(
    0 => array("pipe", "r"),
    1 => array("pipe", "w"),
    2 => array("file", "/tmp/error-output.txt", "a")
);

$startTime = microtime(true);
for ($i = 0; $i < $sizeT; $i++) {
    
    $process = proc_open('./bin/NZMGtransform', $des, $pipes);

    if (is_resource($process)) {
       
        fwrite($pipes[0], $finalPoints[$i]);
        fclose($pipes[0]);

        $coords[$i] = stream_get_contents($pipes[1]);
        fclose($pipes[1]);
        
        $return_value = proc_close($process);
    }
}
$endTime = microtime(true);
echo "grid transform " .($endTime - $startTime) ." seconds<br>";


##############################################################

for ($i = 0; $i < $count; $i ++) {

    $tempID = $ids[$i];
    $tempStructArray[ID] = (string) $ids[$i];
    $tempStructArray[StartDate] = $startDates[$i];
    $tempStructArray[EventCreated] = $eventCreated[$i];
    $tempStructArray[EventModified] = $eventModified[$i];
    $tempStructArray[Description] = (string) $descriptions[$i];
    $tempStructArray[Location] = (string) $locations[$i];
    $tempStructArray[AlternativeRoute] = (string) $alternativeRoute[$i];
    $tempStructArray[EventType] = (string) $eventType[$i];
    $tempStructArray[ExpectedResolution] = (string) $expectedResoultion[$i];
    $tempStructArray[Planned] = (boolean) $planned[$i];
    $tempStructArray[Coordinates] = $coords[$i];

    if (empty($restrictions[$i])) {
        $restrictions[$i] .= "No restrictions apply";
        $tempStructArray[Restrictions] = (string) $restrictions[$i];
    } else {
        $tempStructArray[Restrictions] = (string) $restrictions[$i];
    }

    $tempStructArray[Impact] = (string) $impact[$i];
    $tempStructArray[LocationAreas] = (string) $locationAreas[$i];

    $tempEndDate = $xml->xpath("//tns:roadEvent[tns:eventId=$tempID]/tns:endDate");
    $tempEndDateVal = $tempEndDate[0];
    $tempStructArray[EndDate] = $tempEndDateVal;
    $toBeEncoded[$i] = $tempStructArray;
}

$writer = new XMLWriter();  
$writer->openURI('./SHEvents.xml');   
$writer->startDocument('1.0','UTF-8');
$writer->startElement('TreisInfoResponse');
$writer->setIndent(4);
    
for ($i = 0; $i < $count; $i ++) {
    $writer->startElement('roadEvent');
    $writer->writeElement('eventId', $ids[$i]);
    $writer->writeElement('eventLocation', $locations[$i]);
    $writer->writeElement('eventComments', $descriptions[$i]);
    $writer->writeElement('eventType', $eventTypes[$i]);
    $writer->writeElement('startDate', $startDates[$i]);
    $writer->writeElement('coordinates', $coords[$i]);
    $writer->endElement();
}

$writer->endElement();
$writer->endDocument();   
$writer->flush();

$final = json_encode($toBeEncoded);

file_put_contents("SHEvents.json", $final);

#############################################################################

echo "<br>";


