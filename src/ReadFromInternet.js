var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
      if(this.status == 200)
          callback(this.responseText);
      else
          sendErrorString("Error:Status " + this.status);
  };
  xhr.open(type, url);
  xhr.send();
};

function getWatchVersion()
{
    var platform;
    if(Pebble.getActiveWatchInfo) {
      var watchinfo= Pebble.getActiveWatchInfo();
      platform=watchinfo.platform;
      } else {
        platform="aplite";
      }
    return platform;
}

//if integer is 0 return a string 00. Used for times
function leadingZeroCheck(number)
{
    if (number < 10)
        return "0" + number;
    else
        return number;
}

function getItems(responseText)
{
        // responseText contains a JSON object with item info
        var json = JSON.parse(responseText);
        
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
    
    var monthNames = ["Jan", "Feb", "Mar", "Apr", "May", "Jun",
                      "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"];
        
    
        // Conditions
        var itemNames = "";
        var itemIDs = "";
        var itemDates = "";
        var itemDueDates = "";
        var itemIndentation = "";
        
    
        for(var i=0;i<json.length;i++)
        {
            itemNames = itemNames + json[i].content.replace("|", "") + " |";
            itemIDs = itemIDs  + json[i].id + "|";
            if (json[i].date_string == "null")
                itemDates = itemDates + "|";
            else
                itemDates = itemDates + json[i].date_string + "|";
            itemIndentation = itemIndentation + json[i].indent + "|";
            if (json[i].due_date === null)
            {
                itemDueDates = itemDueDates + "|"; 
            }
            else
            {
                var d = new Date(json[i].due_date);
                //if the time is 23:59 this specifies "no time"
                if ((d.getHours() == 23) && (d.getMinutes() == 59))
                    itemDueDates = itemDueDates + monthNames[d.getMonth()] + " " + d.getDate() + "|";
                else
                    itemDueDates = itemDueDates + monthNames[d.getMonth()] + " " + d.getDate() + " " + d.getHours() + ":" + leadingZeroCheck(d.getMinutes())  + "|";  
            }
            
        }
    
        
    
        var dictionary = 
        {
            "ITEM_NAMES": itemNames,
            "ITEM_IDS": itemIDs,
            "ITEM_DATES": itemDates,
            "ITEM_DUE_DATES": itemDueDates,
            "ITEM_INDENTATION": itemIndentation
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
        json = json.Items;
    
    
        for(var i=0;i<json.length;i++)
        {
            if (json[i].due_date)
            {
                var date = new Date(json[i].due_date);
                //Create timeline array of items that have due dates
                var pin = {
                "id": "TodoistMiniItem-" + json[i].id,
                "time": date.toISOString(),
                "layout": {
                  "type": "genericPin",
                  "title": json[i].content,
                  "tinyIcon": "system://images/SCHEDULED_EVENT"
                  }
                };
                
                insertUserPin(pin, function(responseText) 
                { 
                    
                });
            }
        }   
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
    localStorage.setItem("todoistMiniToken", token);
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
    var projectIndentation = "";
    
    //put today project in (custom)
    projectNames = "Today |";
    projectIDs = "0|";
    projectIndentation = "1|";
    
    for(var i=0;i<json.length;i++)
    {
        projectNames = projectNames + json[i].name.replace("|", "")  + " |";
        projectIDs = projectIDs  + json[i].id + "|";
        projectIndentation = projectIndentation + json[i].indent + "|";
    }

    var dictionary = 
    {
        "PROJECT_NAMES": projectNames,
        "PROJECT_IDs": projectIDs,
        "PROJECT_INDENTATION": projectIndentation
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

function markRecurringItem(responseText)
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

//code 1 = waiting for config
//code 2 = waiting to load data
function sendWaitingMessageAndPerformAction(code)
{
    //when we send app message it just needs to be a 1 or 2 (config or loading)
    var sendCode;
    
    if (code > 2)
        sendCode = 2;
    else
        sendCode = code;
    var dictionary = 
        {
            "WAITING": sendCode
        };
        Pebble.sendAppMessage(dictionary,
                          function(e) 
                          {
                              if (code == 1)
                              {
                                   openConfig();   
                              }
                              if (code == 2)
                              {
                                   getProjectsFromToken(); 
                              }
                              if (code == 3)
                              {
                                   processTodoistDataWithGoogle();
                              }
                              if (code == 4)
                              {
                                   processTodoistData();  
                              }
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

function sendErrorString(errorMsg)
{
    var dictionary = 
        {
            "ERRORMSG": errorMsg
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

function processTodoistData() 
{
    var url = "https://todoist.com/API/login?email=" +
    encodeURIComponent(localStorage.getItem("todoistEmail")) + "&password=" + encodeURIComponent(localStorage.getItem("todoistPassword"));
    localStorage.removeItem("todoistEmail");
    localStorage.removeItem("todoistPassword");
    //note that xhr request is ASYNCHRONOUS everything after it in this function will get executed
    //before it is even finished the next path of execution HAS to be in the callback function
    xhrRequest(url, 'GET', getToken);
}

function processTodoistDataWithGoogle()
{
    var url = "https://todoist.com/API/loginWithGoogle?email=" +
    encodeURIComponent(localStorage.getItem("googleEmail")) + "&oauth2_token=" + encodeURIComponent(localStorage.getItem("googleToken"));
    //note that xhr request is ASYNCHRONOUS everything after it in this function will get executed
    //before it is even finished the next path of execution HAS to be in the callback function
    xhrRequest(url, 'GET', getToken);
}

function getProjectsFromToken()
{
    var url = "https://todoist.com/API/getProjects?token=" + encodeURIComponent(localStorage.getItem("todoistMiniToken"));
    xhrRequest(url, 'GET', getProjects);
}

function getItemsForSelectedProject(projectID)
{
    var url = "https://todoist.com/API/getUncompletedItems?project_id=" +
    encodeURIComponent(projectID) + "&token=" + encodeURIComponent(localStorage.getItem("todoistMiniToken"));
    xhrRequest(url, 'GET', getItems);
}

function getItemsForToday()
{
    var url = "https://todoist.com/API/query?queries=" + encodeURIComponent("[\"Today\"]") + "&token=" + encodeURIComponent(localStorage.getItem("todoistMiniToken"));
    xhrRequest(url, 'GET', getItems);
}

function pinTimelineItems()
{
    //var url = "https://api.todoist.com/API/query?queries=" + encodeURIComponent("[\"No Due Date\"]") + "&token=" + encodeURIComponent(localStorage.getItem("todoistMiniToken"));
    var url = "https://api.todoist.com/API/v6/sync?token=" + encodeURIComponent(localStorage.getItem("todoistMiniToken")) + "&seq_no=" + encodeURIComponent("0") + "&seq_no_global=" + encodeURIComponent("0") + "&resource_types=" + encodeURIComponent("[\"items\"]");
    xhrRequest(url, 'GET', getAllItemsForTimeline);
}

function markItemAsCompleted(itemID)
{
    var url = "https://todoist.com/API/completeItems?ids=" +
    encodeURIComponent("[" + itemID + "]") + "&token=" + encodeURIComponent(localStorage.getItem("todoistMiniToken"));
    var pin = {
                "id": "pin-" + itemID
              };
    if (getWatchVersion() == "basalt")
    {
        //delete pin from timeline if completed
        deleteUserPin(pin, function(responseText) 
        {
            
        });
    }
    
    xhrRequest(url, 'GET', markItem);
}

function markRecurringItemAsCompleted(itemID)
{
    var url = "https://todoist.com/API/updateRecurringDate?ids=" +
    encodeURIComponent("[" + itemID + "]") + "&token=" + encodeURIComponent(localStorage.getItem("todoistMiniToken"));
    xhrRequest(url, 'GET', markRecurringItem);
    if (getWatchVersion() == "basalt")
    {
        //update timeline pins if recurring item completed
        pinTimelineItems();
    }
}


// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
    function(e) 
    {
        //localStorage.removeItem("todoistMiniToken");
        if (localStorage.getItem("todoistMiniToken") === null)
        {
            sendWaitingMessageAndPerformAction(1);
        }
        else
        {
            if (getWatchVersion() == "basalt")
                pinTimelineItems();
            sendWaitingMessageAndPerformAction(2);
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
    if(e.payload.SELECTED_ITEM_RECURRING)
    {
        markRecurringItemAsCompleted(e.payload.SELECTED_ITEM_RECURRING);
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
    
    //pebble does not seem to handle encoded %2B properly (makes it a space instead of a plus sign)
    //Possibly replace spaces with plus signs... unfortunately this would screw up anything that actually had a space in it (a password for example)
    
    //console.log("response: " + e.response);
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
        localStorage.setItem("googleEmail", loginData.email);
        sendWaitingMessageAndPerformAction(3);
        
    }
    else
    {
        
        localStorage.setItem("todoistEmail", loginData.email);
        localStorage.setItem("todoistPassword", loginData.password);
        sendWaitingMessageAndPerformAction(4);
    }

}
// Listen for a configuration request
Pebble.addEventListener('showConfiguration', openConfig);

//Listen for configuration window closing
Pebble.addEventListener('webviewclosed', closeConfig);






/******************************* timeline lib *********************************/

// The timeline public URL root
var API_URL_ROOT = 'https://timeline-api.getpebble.com/';

/**
 * Send a request to the Pebble public web timeline API.
 * @param pin The JSON pin to insert. Must contain 'id' field.
 * @param type The type of request, either PUT or DELETE.
 * @param callback The callback to receive the responseText after the request has completed.
 */
function timelineRequest(pin, type, callback) {
  // User or shared?
  var url = API_URL_ROOT + 'v1/user/pins/' + pin.id;

  // Create XHR
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);

  // Get token
  Pebble.getTimelineToken(function(token) {
    // Add headers
    xhr.setRequestHeader('Content-Type', 'application/json');
    xhr.setRequestHeader('X-User-Token', '' + token);

    // Send
    xhr.send(JSON.stringify(pin));
  }, function(error) { sendErrorString("Timeline Error: " + error); });
}

/**
 * Insert a pin into the timeline for this user.
 * @param pin The JSON pin to insert.
 * @param callback The callback to receive the responseText after the request has completed.
 */
function insertUserPin(pin, callback) {
  timelineRequest(pin, 'PUT', callback);
}

/**
 * Delete a pin from the timeline for this user.
 * @param pin The JSON pin to delete.
 * @param callback The callback to receive the responseText after the request has completed.
 */
function deleteUserPin(pin, callback) {
  timelineRequest(pin, 'DELETE', callback);
}

/***************************** end timeline lib *******************************/