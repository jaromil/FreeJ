// freej script for a simple text scroller
// it makes smart use of arrays as FIFO pipes
// and the file_to_strings(filename) function
// to read a text file into an array of strings

// (C)2005 Denis Jaromil Rojo - GNU GPL 

W = 640;
H = 480;

set_resolution(W, H);
set_fps(25);

wordspacing = 5;

// setup the keyboard quit
running = true;
kbd = new KeyboardController();
kbd.released_esc = function() { running = false; }
register_controller( kbd );

// read a text file into an array
words = file_to_strings("test.txt");

// debug
echo ( words.length + " words read" );

idx = 0;
// words.length property says how big it is
// access words with words[0] words[1] and so on


// make a black background
/*
background = new GeometryLayer();
background.color(0x00000000);
background.set_blit("alpha");
background.set_blit_value(0.2);
background.rectangle(0, 0, background.w(), background.h() );
add_layer( background );
*/

// setup the array of rendered words
// each one is a TextLayer object
scroll = new Array();

/*
  scroll[] is an Array object
  we will use it as a FIFO pipe, thru the push() and shift()
  methods of javascript arrays:

   t(n): scroll.push( txt ) pushes in text from the right (here below)
   array scroll  [ 0 ] [ 1 ] [ 2 ] [ 3 ] [ 4 ] [ 5 ]     _/
   screen -> | <- bla - bla - bla - bla - bla - bla - | <- screen
   
   t(n+1):
       [ 0 ] | <- [ 1 ] - [ 2 ] - [ 3 ] ...
       scroll.shift() returns scroll[0] (which we delete)
       
  so let's go on...
*/

function render_word(wrd) {

    lay = new TextLayer();

    lay.set_fps(25);

    lay.start();

    add_layer( lay ); // add it to the screen

    lay.set_position( W, 400 ); // start from the right of the scree

    lay.print( wrd ); // print the string in the layer

    lay.size( 50 ); // set the size

    //    lay.slide_position( 0 - lay.w() , 200, 2); // slide to the right

    return lay;
}
  
// MAIN

// create a new TextLayer with the word
txt = render_word( words[idx] );

// append it as last element of the scroll[] array
scroll.push( txt );

// advance the index of words
idx++;

trigger = new TriggerController();
register_controller( trigger );
trigger.frame = function() {


    // slide all to the left
    for(i=0; i<scroll.length; i++) {
	echo("- " + i + " is  at " + scroll[i].x() + "x" + scroll[i].y());

	scroll[i].set_position( scroll[i].x() - 2, scroll[i].y() );
	echo("- " + i + " now at " + scroll[i].x() + "x" + scroll[i].y());
	
    }

    
    // check if the leftmost is out of screen
    if( scroll[0].x() + scroll[0].w() < 0) {
	// pull it out from the array
	txt = scroll.shift();
	// remove it from the screen
	rem_layer( txt );
	// delete it
	delete txt;
    }


    // check if the rightmost all entered the screen
    rightmost = scroll[ scroll.length -1 ];
    if( rightmost.x() + rightmost.w() <  W - wordspacing ) {
	// then we need a new one on the left
	
	if ( words[idx].length > 0 ) {
		// create a new TextLayer with the word
		txt = render_word( words[idx] );

		// append it as last element of the scroll[] array
		scroll.push( txt );
	}
	
	// advance the index
	idx++;
    }

    // if the words are finished: stop
    if(idx >= words.length)
	this.frame = function() { };
    
}

