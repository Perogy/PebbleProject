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
    // responseText contains a JSON object with item info
    var json = JSON.parse(responseText);
    console.log("\nGET ITEMS RESPONSE:" + responseText);
    
    //check if query was a "Today" query and go into the "data" section if it is
    if (json[0])
    {
        if (json[0].hasOwnProperty("query"))
        {
            json = json[0].data;
        }
    }
    
    if (json[0])
    {
        if (!json[0].hasOwnProperty("id"))
        {
            sendErrorMessage(3);
            return;
        }
    }
    
    // Conditions
    var itemNames = "";
    var itemIDs = "";
    for(var i=0;i<json.length;i++)
    {
        itemNames = itemNames + json[i].content + " |";
        itemIDs = itemIDs  + json[i].id + "|";
    }

    var dictionary = 
    {
        "ITEM_NAMES": itemNames,
        "ITEM_IDS": itemIDs
    };

    // Send to Pebble
    Pebble.sendAppMessage(dictionary,
                          function(e) 
                          {
                              
                          },
                          function(e) 
                          {
                              console.log("Data did not transfer to pebble successfully");
                          });   
}

function getAllItemsForTimeline(responseText)
{
    // responseText contains a JSON object with item info
    var json = JSON.parse(responseText);
    
    // Conditions
    var itemNames = "";
    var itemIDs = "";
    for(var i=0;i<json.length;i++)
    {
        itemNames = itemNames + json[i].content + " |";
        itemIDs = itemIDs  + json[i].id + "|";
    }

    var dictionary = 
    {
        "TIMELINE_JSON": json
    };

    // Send to Pebble
    Pebble.sendAppMessage(dictionary,
                          function(e) 
                          {
                              
                          },
                          function(e) 
                          {
                              console.log("Data did not transfer to pebble successfully");
                          });   
}

function getToken(responseText) 
{
    // responseText contains a JSON object with token info
    var json = JSON.parse(responseText);
    if (responseText == "\"LOGIN_ERROR\"")
    {
        sendErrorMessage(1);
        openConfig();
        return;
    }
    // Conditions
    var token = json.token;
    localStorage.setItem("token", token);
    getProjectsFromToken();
}

function getProjects(responseText)
{
    // responseText contains a JSON object with project data
    var json = JSON.parse(responseText);

    if (json[0])
    {
        if (!json[0].hasOwnProperty("id"))
        {
            sendErrorMessage(2);
            return;
        }
    }
    // Conditions
    var projectNames = "";
    var projectIDs = "";
    
    //put today project in (custom)
    projectNames = "Today |";
    projectIDs = "0|";
    
    for(var i=0;i<json.length;i++)
    {
        projectNames = projectNames + json[i].name + " |";
        projectIDs = projectIDs  + json[i].id + "|";
    }

    var dictionary = 
    {
        "PROJECT_NAMES": projectNames,
        "PROJECT_IDs": projectIDs
    };

    // Send to Pebble
    Pebble.sendAppMessage(dictionary,
                          function(e) 
                          {
                              
                          },
                          function(e) 
                          {
                              console.log("Data did not transfer to pebble successfully");
                          });   
}

function markItem(responseText)
{
    if (responseText == "\"ok\"")
    {
        var dictionary = 
        {
            "SELECTED_ITEM": "1"
        };
        Pebble.sendAppMessage(dictionary,
                          function(e) 
                          {
                              
                          },
                          function(e) 
                          {
                              console.log("Data did not transfer to pebble successfully");
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
                              
                          },
                          function(e) 
                          {
                              console.log("Data did not transfer to pebble successfully");
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
                              
                          },
                          function(e) 
                          {
                              console.log("Data did not transfer to pebble successfully");
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
                              
                          },
                          function(e) 
                          {
                              console.log("Data did not transfer to pebble successfully");
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
    var url = "https://todoist.com/API/loginWithGoogle?email=" +
    localStorage.getItem("email") + "&oauth2_token=" + localStorage.getItem("googleToken");
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

function getItemsForToday()
{
    var url = "https://api.todoist.com/API/query?queries=[\"Today\"]&token=" + localStorage.getItem("token");
    xhrRequest(url, 'GET', getItems);
}

function getTimelineItemsFor7Days()
{
    var url = "https://api.todoist.com/API/query?queries=[\"7 days\"]&token=" + localStorage.getItem("token");
    xhrRequest(url, 'GET', getAllItemsForTimeline);
}

function markItemAsCompleted(itemID)
{
    var url = "https://todoist.com/API/completeItems?ids=[" +
    itemID + "]&token=" + localStorage.getItem("token");
    xhrRequest(url, 'GET', markItem);
}


// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
    function(e) 
    {
        //temporary to test what happens when there is no token
        //localStorage.removeItem("token");
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
    }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    if(e.payload.SELECTED_PROJECT)
    {
        if (e.payload.SELECTED_PROJECT == "0")
            getItemsForToday();
        else
            getItemsForSelectedProject(e.payload.SELECTED_PROJECT);
    }
    if(e.payload.SELECTED_ITEM)
    {
        markItemAsCompleted(e.payload.SELECTED_ITEM);
    }
  }                     
);

//sets the configuration options from the config page that the user has just saved.
function setConfig(loginData)
{
    localStorage.setItem("ConfigData", JSON.stringify(loginData));
    var configString = loginData.scrollSpeed + '|' + loginData.backgroundColor + '|' + loginData.foregroundColor + '|' + loginData.altBackgroundColor + '|' + loginData.altForegroundColor + '|' + loginData.highlightBackgroundColor + '|' + loginData.highlightForegroundColor + '|';
    

    var dictionary = 
    {
        "CONFIG": configString
    };

    // Send to Pebble
    Pebble.sendAppMessage(dictionary,
                          function(e) 
                          {
                              
                          },
                          function(e) 
                          {
                              console.log("Data did not transfer to pebble successfully");
                          });   
}

function openConfig(e) 
{
    if (localStorage.getItem("ConfigData") === null)
    {
        Pebble.openURL('https://perogy.github.io/PebbleProject/index.html');
    }
    else
    {
        var configData = JSON.parse(localStorage.getItem("ConfigData"));
        Pebble.openURL('https://perogy.github.io/PebbleProject/index.html#' + 'scrollSpeed=' + configData.scrollSpeed + '&backgroundColor=' + configData.backgroundColor + '&foregroundColor=' + configData.foregroundColor + '&altBackgroundColor=' + 
                                                            configData.altBackgroundColor + '&altForegroundColor=' + configData.altForegroundColor + '&highlightBackgroundColor=' + configData.highlightBackgroundColor + '&highlightForegroundColor=' + configData.highlightForegroundColor);  
    }
}

function closeConfig(e) {
    
    var loginData = JSON.parse(decodeURIComponent(e.response));
    
    if (loginData.type == "configData")
    {
        setConfig(loginData);
        return;
    }
    
    if (loginData.googleToken)
    {
        //check whether google or normal login and then run appropriate code
        localStorage.setItem("googleToken", loginData.token);
        localStorage.setItem("email", loginData.email);
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