$(function() {
  var websocket;
  try {
    websocket = new WebSocket("ws://" + window.location.host + "/");
  } catch (e) {
    alert("Unable to connect to ws://" + window.location.host + "/");
  }
  var animations = [];
  var isAudioDoneLoading = false;
  var isVideoDoneLoading = false;
  var isAudioDonePlaying = true;
  var isVideoDonePlaying = true;
  var readyToPlay = false;
  var playing = false;
  var currentAnimation = null;
  var currentIndex = false;

  /**
   * Called when the audio or video is done loading, and if
   * both are done then enables the play button.
   */
  var animationDoneLoading = function() {
    $("#currentanimationtitle").text(currentAnimation.title);
    console.log("Animation done loading: Audio: " + isAudioDoneLoading + ", Video: " + isVideoDoneLoading);

    if (isAudioDoneLoading == true && isVideoDoneLoading == true) {
      readyToPlay = true;
      $("#playbutton").removeClass('ui-state-disabled');
      $("#currentanimationpos").progressbar('option', 'max', $("#audioplayer")[0].duration);
      $("#currentanimationpos").progressbar('value', 0);
    } else {
      $("#currentanimationpos").progressbar('value', false);
      $("#playbutton span").removeClass('ui-icon-stop');
      $("#playbutton span").addClass('ui-icon-play');
      $("#playbutton").addClass("ui-state-disabled");
      //playing = false;
    }
  };

  animationDonePlaying = function() {
    if (isAudioDonePlaying == true && isVideoDonePlaying == true) {
      readyToPlay = true;
      $("#playbutton").removeClass('ui-state-disabled');
      $("#currentanimationpos").progressbar('option', 'max', $("#audioplayer")[0].duration);
      $("#currentanimationpos").progressbar('value', 0);
      console.log("Load the next animation");
    } else {
      $("#currentanimationpos").progressbar('value', false);
      $("#playbutton span").removeClass('ui-icon-stop');
      $("#playbutton span").addClass('ui-icon-play');
      $("#playbutton").addClass("ui-state-disabled");
    }
  };

  /** 
   * Initialize the audio and video loading
   */
  var loadAnimation = function(index) {
    //playing = false;
    currentAnimation = animations[index];
    isAudioDoneLoading = false;
    isVideoDoneLoading = false;
    websocket.send(JSON.stringify({type:"load", index: animations.indexOf(currentAnimation)}));
    console.log("Loading music: " + currentAnimation.music);
    if (currentAnimation.music == undefined) {
      isAudioDoneLoading = true;
      $('#audioplayer').attr('src', '');
      animationDoneLoading();
    } else {
      $('#audioplayer').attr('src', currentAnimation.music);
      $('#audioplayer')[0].load();
    }
  };
  
  // Websocket ----------------------------------------------------------
  websocket.onopen = function(ev) {
    $("#serverstatus").text("Connected");
  };
  websocket.onclose = function(ev) {
    $("#serverstatus").text("Disconnected");
    alert("Server disconnected");
  };
  websocket.onmessage = function(ev) {
    var msg = JSON.parse(ev.data);
    console.log("Recieved: " + ev.data);
    switch (msg.type) {
      case 'pong':
        $("#pingdiv").text("Ping... Pong");
        window.setTimeout(function() { $("#pingdiv").text(''); }, 2000);
        break;
      case 'animationlist':
        $("#animationlist").empty();
        animations = msg.animations;
        var length = msg.animations.length;
        for (var i = 0; i < length; i++) {
          msg.animations.id = i;
          $("#animationlist").append('<li id="animation' + i +  '" class="ui-state-default ui-corner-all"><div class="handle"><span class="ui-icon ui-icon-carat-2-n-s"></span></div>' + msg.animations[i].title + '</li>');
        }
        break;
      case 'error':
        alert("Server error: " + msg.message);
        break;
      case 'doneloading':
        isVideoDoneLoading = true;
        animationDoneLoading(); 
        break;
      case 'doneplaying':
        isVideoDonePlaying = true;
        animationDonePlaying();
      default:
        alert('Unrecognized server message: ' + ev.data);
        break;
    }
  };
  websocket.onerror = function(ev) {
    $("#serverstatus").text("Error: " + ev.data);
    alert("Websocket error: " + ev.data);
  };

  // Animation list -----------------------------------------------
  $("#animationlist")
    .sortable({ handle: ".handle"})
    .selectable({ 
      tolerance: 'fit',
      selected: function(e,u) {
        // a new aniimation was selected
        $(u.selected).addClass('ui-state-active'); 
        currentIndex = parseInt(u.selected.id.substring(9, u.selected.id.length), 10)
        if (playing == false) {
          // If an animation isn't currently playing load the selected one. 
          loadAnimation(currentIndex);
        }
      },
      unselected: function(e,u) { $(u.unselected).removeClass('ui-state-active'); }
    })
  $("#refreshbutton")
    .hover(
      function() { $(this).addClass("ui-state-hover");},
      function() {$(this).removeClass("ui-state-hover");}
    )
    .mousedown(function() { $(this).addClass("ui-state-active"); })
    .mouseup(function() { $(this).removeClass("ui-state-active"); })
    .click(function() {
      readyToPlay = false;
      websocket.send(JSON.stringify({type:"getanimations"})); 
      $("#animationlist").empty();
    });

  // Audio player---------------------------------------------------
  // Send the play signal to the server when the audio starts playing
  $("#audioplayer").bind('play', function() { 
    console.log("play " + animations.indexOf(currentAnimation));
    websocket.send(JSON.stringify({type:"play", index: animations.indexOf(currentAnimation)}));
  });
  // Catch when the audio is done loading
  $("#audioplayer").bind('canplaythrough', function() { 
    isAudioDoneLoading = true;
    console.log("Audio can play through");
    animationDoneLoading();
  });
  $("#audioplayer").bind('timeupdate', function() {
    $("#currentanimationpos").progressbar('value', $("#audioplayer")[0].currentTime);
  });
  $("#audioplayer").bind('ended', function() {
    isAudioDonePlaying = true;
    animationDonePlaying();
  });
  $("#playbutton")
    .hover(
      function() {
        if (readyToPlay) {
          $(this).addClass("ui-state-hover");
        }
      },
      function() { $(this).removeClass("ui-state-hover"); }
    )
    .mousedown(function() {
      if (readyToPlay) {
        $(this).addClass("ui-state-active");
      }
    })
    .mouseup(function() {
      //$(this).removeClass("ui-state-active"); should be when the animation stops
    })
    .click(function() {
      if (playing == false) {
        if (readyToPlay) {
          $("#audioplayer")[0].play();
          $("#playbutton span").removeClass('ui-icon-play');
          $("#playbutton span").addClass('ui-icon-stop');
          playing = true;
        }
      } else {
        if (currentAnimation.music != undefined) {
          $("#audioplayer")[0].pause();
          $("#audioplayer")[0].currentTime = 0;
        }
        $("#playbutton span").removeClass('ui-icon-stop');
        $("#playbutton span").addClass('ui-icon-play');
        playing = false;

        websocket.send(JSON.stringify({type:"stop"}));

        // Load the currently selected animation
        if (animations[currentIndex] != currentAnimation) {
          loadAnimation(currentIndex);
        }
      }
    })
  $("#currentanimationpos").progressbar({value: 0, max: 1});

  // Configuration tab -------------------------------------------
  $("#tabs").tabs();
  $("#lagslider").slider({value: [50]});
  $("#pingbutton").button();
  $("#pingbutton").click(function() {
    websocket.send(JSON.stringify({type:"ping"}));
    $("#pingdiv").text("Ping...");
  });
});

