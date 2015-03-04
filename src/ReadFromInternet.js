var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};


function getItems(responseText)
{
    console.log("IN Items");
    // responseText contains a JSON object with weather info
    var json = JSON.parse(responseText);

    console.log("\nitemresponse:" + responseText + "\n");
    // Conditions
    var itemNames = "";
    var itemIDs = "";
    for(var i=0;i<json.length;i++)
    {
        itemNames = itemNames + json[i].content + "|";
        itemIDs = itemIDs  + json[i].id + "|";
    }
    console.log("\nitemNames:\n" + itemNames);
    console.log("\nitemIDs:\n" + itemIDs);

    var dictionary = 
    {
        "ITEM_NAMES": itemNames,
        "ITEM_IDS": itemIDs
    };

    // Send to Pebble
    Pebble.sendAppMessage(dictionary,
                          function(e) 
                          {
                              console.log("info sent to Pebble successfully!");
                          },
                          function(e) 
                          {
                              console.log("Error sending weather info to Pebble!");
                          });   
}

function getToken(responseText) 
{
    // responseText contains a JSON object with weather info
    var json = JSON.parse(responseText);

    // Conditions
    var token = json.token;
    console.log("\ndat token:" + token + "\n");
    localStorage.setItem("token", token);
    token = localStorage.getItem("token");
    var url = "https://todoist.com/API/getProjects?token=" + token;
    xhrRequest(url, 'GET', getProjects);
}

function getProjects(responseText)
{
    console.log("IN PROJECTS");
    // responseText contains a JSON object with weather info
    var json = JSON.parse(responseText);

    // Conditions
    var projectNames = "";
    var projectIDs = "";
    for(var i=0;i<json.length;i++)
    {
        projectNames = projectNames + json[i].name + "|";
        projectIDs = projectIDs  + json[i].id + "|";
    }
    console.log("\nProjectNames:\n" + projectNames);
    console.log("\nProjectIDs:\n" + projectIDs);

    var dictionary = 
    {
        "PROJECT_NAMES": projectNames,
        "PROJECT_IDs": projectIDs
    };

    // Send to Pebble
    Pebble.sendAppMessage(dictionary,
                          function(e) 
                          {
                              console.log("info sent to Pebble successfully!");
                          },
                          function(e) 
                          {
                              console.log("Error sending weather info to Pebble!");
                          });   
}



function processTodoistData() 
{
    var url = "https://todoist.com/API/login?email=" +
    "bpaugh@hotmail.com" + "&password=" + "aragorn123";
    //note that xhr request is ASYNCHRONOUS everything after it in this function will get executed
    //before it is even finished the next path of execution HAS to be in the callback function
    xhrRequest(url, 'GET', getToken);
}

function getItemsForSelectedProject(projectID)
{
    var url = "https://todoist.com/API/getUncompletedItems?project_id=" +
    projectID + "&token=" + localStorage.getItem("token");
    console.log("\nprojID:" + url + "\n");
    xhrRequest(url, 'GET', getItems);
}


// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!");
    processTodoistData();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log("AppMessage received!");
    if(e.payload.selectedproject)
    {
          getItemsForSelectedProject(e.payload.selectedproject);
    }
  }                     
);

