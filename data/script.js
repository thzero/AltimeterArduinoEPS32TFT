function atmosphere(json, editable) {
  let altitudeASLEl = document.getElementById('altitudeASL');
  if (editable)
    altitudeASLEl.value = Number(json.altitudeASL).toFixed(2);
  else
    altitudeASLEl.innerHTML = Number(json.altitudeASL).toFixed(2);
  let pressureAboveEl = document.getElementById('pressureAbove');
  if (editable)
    pressureAboveEl.value = Number(json.pressureAbove).toFixed(2);
  else
    pressureAboveEl.innerHTML = Number(json.pressureAbove).toFixed(2);
  let pressureAtEl = document.getElementById('pressureAt');
  if (editable)
    pressureAtEl.value = Number(json.pressureAt).toFixed(2);
  else
    pressureAtEl.innerHTML = Number(json.pressureAt).toFixed(2);
  let temperatureEl = document.getElementById('temperature');
  if (editable)
    temperatureEl.value = Number(json.temperature).toFixed(2);
  else
    temperatureEl.innerHTML = Number(json.temperature).toFixed(2);
}

var batteries = null;
function battery_level(json) {
  let batteryEl = document.getElementById('battery');

  let battery_charging = document.getElementById('battery-charging');
  let battery_empty = document.getElementById('battery-empty');
  let battery_full = document.getElementById('battery-full');
  let battery_full_charging = document.getElementById('battery-full-charging');
  let battery_mostly = document.getElementById('battery-mostly');
  let battery_partial = document.getElementById('battery-partial');
  if (!batteries) {
    batteries = [];
    batteries.push(battery_charging);
    batteries.push(battery_empty);
    batteries.push(battery_full);
    batteries.push(battery_full_charging);
    batteries.push(battery_mostly);
    batteries.push(battery_partial);
  }
  for (let item of batteries)
    item.style.display = 'none';

  let batteryVoltage = json.monitorBatteryVoltage;
  let batteryVoltageMax = json.monitorBatteryVoltageMax;

  let batteryLevelCd = 'full';
  if (batteryVoltage < batteryVoltageMax / 3)
    batteryLevelCd = 'empty';
  else if (batteryVoltage < batteryVoltageMax / 2)
    batteryLevelCd = 'partial';
  else if (batteryVoltage < batteryVoltageMax * 2 / 3)
    batteryLevelCd = 'mostly';

  let batteryVoltageEl = document.getElementById('battery-voltage');
  batteryVoltageEl.innerHTML = batteryVoltage;
  let batterylevelEl = document.getElementById('battery-' + batteryLevelCd);
  // let batterylevelEl = document.getElementById('battery-mostly');
  if (batterylevelEl)
    batterylevelEl.style.display = '';

  let battery_status = json.monitorBatteryStatus;
  batteryEl.style.display = battery_status ? '' : 'none';
}

function header(json) {
  let titleEl = document.getElementById('title');
  titleEl.innerHTML = json.title;
  let versionEl = document.getElementById('version');
  versionEl.innerHTML = json.version;
  let copyrightEl = document.getElementById('copyright');
  copyrightEl.innerHTML = json.copyright + ' ' + json.copyrightYears;
}

function launch(json, editable) {
  let launchDetectEl = document.getElementById('launchDetect');
   if (editable)
    launchDetectEl.value = json.launchDetect;
  else
    launchDetectEl.innerHTML = json.launchDetect;
}

function memory(json) {
  let memoryEl = document.getElementById('memoryFree');
  memoryEl.innerHTML = json.monitorMemoryFree;
}

function samples(json, editable) {
  let samplesAscentEl = document.getElementById('samplesAscent');
  if (editable)
    samplesAscentEl.value = json.samplesAscent;
  else
    samplesAscentEl.innerHTML = json.samplesAscent;
  let samplesDescentEl = document.getElementById('samplesDescent');
  if (editable)
    samplesDescentEl.value = json.samplesDescent;
  else
    samplesDescentEl.innerHTML = json.samplesDescent;
  let samplesGroundEl = document.getElementById('samplesGround');
  if (editable)
    samplesGroundEl.value = json.samplesGround;
  else
    samplesGroundEl.innerHTML = json.samplesGround;
}

function wifi(json, editable) {
  // let wifiAddressEl = document.getElementById('wifiAddress');
  // wifiAddressEl.innerHTML = json.wifiAddress;
  let wifiSSIDEl = document.getElementById('wifiSSID');
  if (editable)
    wifiSSIDEl.value = json.wifiSSID;
  else
    wifiSSIDEl.innerHTML = json.wifiSSID;
}

function requestGet(url, callback) {
  /*
  let jsonDebug = {"altitudeASL":1003.247,"pressureAbove":83.63346,"pressureAt":1003.247,"temperature":31.45951,"title":"AltimeterEPS32TFT","copyright":"(C) thZero.com","copyrightYears":"2024-2024","version":"0.1","launchDetect":20,"monitorBatteryStatus":false,"monitorBatteryVoltage":"3.5","monitorBatteryVoltageMax":"3.7","monitorMemoryFree":1628,"samplesAscent":21,"samplesDescent":3,"samplesGround":8,"wifiAddress":"192.168.1.4","wifiSSID":"F1F618"};
  set(jsonDebug);
  */
  
  let xhr = new XMLHttpRequest();
  xhr.responseType = 'json';
  xhr.open('GET', url, true);
  xhr.onload  = function() {
    if (xhr.status >= 200 && xhr.status < 300) {
      let jsonResponse = xhr.response;
      // console.log(JSON.stringify(jsonResponse));
      if (callback)
        callback(jsonResponse);
    }
  };
  xhr.send(null);
}

function requestPostJSON(url, json, callback) {
  let xhr = new XMLHttpRequest();
  // listen for `load` event
  xhr.onload = () => {
    if (xhr.status >= 200 && xhr.status < 300) {
      let jsonResponse = req.response;
      // console.log(JSON.stringify(jsonResponse));
      if (callback)
        callback(jsonResponse);
    }
  };

  // open request
  xhr.open('POST', '/settings/save')
  // set `Content-Type` header
  xhr.setRequestHeader('Content-Type', 'application/json')
  // send rquest with JSON payload
  xhr.send(JSON.stringify(json))
}