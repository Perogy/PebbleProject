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
    // responseText contains a JSON object with item info
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
                              console.log("Error sending info to Pebble!");
                          });   
}

function getToken(responseText) 
{
    // responseText contains a JSON object with token info
    var json = JSON.parse(responseText);
    if (responseText == "\"LOGIN_ERROR\"")
    {
        console.log("\ngot login error");
        sendErrorMessage(1);
        openConfig();
        return;
    }
    // Conditions
    var token = json.token;
    console.log("\ndat token:" + token + "\n");
    localStorage.setItem("token", token);
    getProjectsFromToken();
}

function getProjects(responseText)
{
    console.log("IN PROJECTS");
    // responseText contains a JSON object with project data
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
                              console.log("Error sending info to Pebble!");
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

//code 1 = Login failed
function sendErrorMessage(code)
{
    var dictionary = 
        {
            "ERROR": code
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

function processTodoistData(email, password) 
{
    var url = "https://todoist.com/API/login?email=" +
    email + "&password=" + password;
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
    //localStorage.removeItem("token");
    if (localStorage.getItem("token") === null)
    {
        sendWaitingMessage(1);
        console.log("Opening Config: token = " + localStorage.getItem("token"));
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
        console.log("PAYLOAD SELECTED PROJECT");
        getItemsForSelectedProject(e.payload.SELECTED_PROJECT);
    }
    if(e.payload.SELECTED_ITEM)
    {
        console.log("PAYLOAD SELECTED ITEM");
        markItemAsCompleted(e.payload.SELECTED_ITEM);
    }
  }                     
);

//sets the configuration options from the config page that the user has just saved.
function setConfig(loginData)
{
    console.log("IN Config");
    localStorage.setItem("ConfigData", JSON.stringify(loginData));
    var configString = loginData.scrollSpeed + '|' + loginData.backgroundColor + '|' + loginData.foregroundColor + '|' + loginData.altBackgroundColor + '|' + loginData.altForegroundColor + '|' + loginData.highlightBackgroundColor + '|' + loginData.highlightForegroundColor + '|';
    

    console.log("configstring: " + configString + "\n");
    var dictionary = 
    {
        "CONFIG": configString
    };

    // Send to Pebble
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

function openConfig(e) {
    //if (localStorage.getItem("ConfigData") === null)
    //{
        Pebble.openURL('http://52.10.200.175/index.html');
    //}
    //else
    //{
        //var configData = JSON.parse(localStorage.getItem("ConfigData"));
        //console.log('\nconfig member:' + configData.backgroundColor);
        //console.log('\nhttp://52.10.200.175/index.html#' + JSON.stringify(configData));
        //Pebble.openURL('http://52.10.200.175/index.html#' + 'scrollSpeed=' + configData.scrollSpeed + '&backgroundColor=' + configData.backgroundColor + '&foregroundColor=' + configData.foregroundColor + '&altBackgroundColor=' + 
        //                                                    configData.altBackgroundColor + '&altForegroundColor=' + configData.altForegroundColor + '&hightlightBackgroundColor=' + configData.highlightBackgroundColor + '&highlightForegroundColor=' + configData.highlightForegroundColor);  
    //}
}

function closeConfig(e) {
    
    var loginData = JSON.parse(decodeURIComponent(e.response));
    console.log('Configuration window returned: ', JSON.stringify(loginData));
    
    if (loginData.type == "configData")
    {
        setConfig(loginData);
        return;
    }
    
    if (loginData.googleToken)
    {
        //check whether google or normal login and then run appropriate code
        localStorage.setItem("googleToken", loginData.token);
        sendWaitingMessage(2);
        processTodoistDataWithGoogle();
    }
    else
    {
        sendWaitingMessage(2);
        processTodoistData(loginData.email, loginData.password);
    }

}
// Listen for a configuration request
Pebble.addEventListener('showConfiguration', openConfig);

//Listen for configuration window closing
Pebble.addEventListener('webviewclosed', closeConfig);
