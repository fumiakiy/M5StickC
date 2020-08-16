function writeData(ar) {
  const d = new Date()
  const aar = [...ar, d.getTime(), d.toLocaleString()]
  const sheet = SpreadsheetApp.getActiveSheet();
  sheet.insertRowBefore(1);
  sheet.getRange(1, 1, 1, aar.length).setValues([aar])
}

function doPost(e) {
  const d = e.postData.contents;
  const j = JSON.parse(d);
  writeData(j);
  return ContentService.createTextOutput("ok");
}