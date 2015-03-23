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
        itemNames = itemNames + json[i].content + " |";
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

    console.log("\nTOKEN RESPONSE: " + responseText);
    // Conditions
    var token = json.token;
    console.log("\ndat token:" + token + "\n");
    localStorage.setItem("token", token);
    getProjectsFromToken();
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
        projectNames = projectNames + json[i].name + " |";
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

function markItem(responseText)
{
    console.log("IN MARK:" + responseText);
    if (responseText == "\"ok\"")
    {
        var dictionary = 
        {
            "SELECTED_ITEM": "1"
        };
        Pebble.sendAppMessage(dictionary,
                          function(e) 
                          {
                              console.log("info sent to Pebble successfully!");
                          },
                          function(e) 
                          {
                              console.log("Error sending info to Pebble!");
                          });   
    }
    else
    {
        var dictionary = 
        {
            "SELECTED_ITEM": "0"
        };
        Pebble.sendAppMessage(dictionary,
                          function(e) 
                          {
                              console.log("info sent to Pebble successfully!");
                          },
                          function(e) 
                          {
                              console.log("Error sending info to Pebble!");
                          });   
    }
    
}

//code 1 = waiting for config
//code 2 = waiting to load data
function sendWaitingMessage(code)
{
    var dictionary = 
        {
            "WAITING": code
        };
        Pebble.sendAppMessage(dictionary,
                          function(e) 
                          {
                              console.log("info sent to Pebble successfully!");
                          },
                          function(e) 
                          {
                              console.log("Error sending info to Pebble!");
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

function processTodoistDataWithGoogle()
{
    console.log("\nGETGOOGLETOKEN:" + localStorage.getItem("googleToken") + "\n");
    var url = "https://todoist.com/API/loginWithGoogle?email=" +
    "bradpaugh@gmail.com" + "&oauth2_token=" + localStorage.getItem("googleToken");
    //note that xhr request is ASYNCHRONOUS everything after it in this function will get executed
    //before it is even finished the next path of execution HAS to be in the callback function
    xhrRequest(url, 'GET', getToken);
}

function getProjectsFromToken()
{
    var url = "https://todoist.com/API/getProjects?token=" + localStorage.getItem("token");
    xhrRequest(url, 'GET', getProjects);
}

function getItemsForSelectedProject(projectID)
{
    var url = "https://todoist.com/API/getUncompletedItems?project_id=" +
    projectID + "&token=" + localStorage.getItem("token");
    console.log("\nprojID:" + url + "\n");
    xhrRequest(url, 'GET', getItems);
}

function markItemAsCompleted(itemID)
{
    var url = "https://todoist.com/API/completeItems?ids=[" +
    itemID + "]&token=" + localStorage.getItem("token");
    xhrRequest(url, 'GET', markItem);
}


// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!");
    //temporary to test what happens when there is no token
    localStorage.removeItem("token");
    if (localStorage.getItem("token") === null)
    {
        sendWaitingMessage(1);
        openConfig();
    }
    else
    {
        sendWaitingMessage(2);
        getProjectsFromToken();   
    }
    //check if there is login information
    //check if token has expired
    //if token expired bring to authorization screen on phone and state on watch app "waiting for authorization" or something similar
    //if not expired pull in data
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log("AppMessage received! " + e.payload.SELECTED_PROJECT + e.payload.SELECTED_ITEM);
    if(e.payload.SELECTED_PROJECT)
    {
          getItemsForSelectedProject(e.payload.SELECTED_PROJECT);
    }
    if(e.payload.SELECTED_ITEM)
    {
          markItemAsCompleted(e.payload.SELECTED_ITEM);
    }
  }                     
);

function openConfig(e) {
    // Show config page
  Pebble.openURL('http://52.10.200.175/index.html');
}

function closeConfig(e) {
    //var configuration = JSON.parse(decodeURIComponent(e.response));
    console.log('Configuration window returned: ' + e.response);
      localStorage.setItem("googleToken", e.response);
      processTodoistDataWithGoogle();
}
// Listen for a configuration request
Pebble.addEventListener('showConfiguration', openConfig);

//Listen for configuration window closing
Pebble.addEventListener('webviewclosed', closeConfig);
