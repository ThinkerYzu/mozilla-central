/*
 * Test for PNG encoding in libpr0n
 *
 */

const Ci = Components.interfaces;
const Cc = Components.classes;

var png1A = {
	// A 3x3 image, rows are red, green, blue.
	// RGB format, transparency defaults.

	transparency : null,

	frames  : [
		{
			width  : 3,    height : 3,

			format : Ci.imgIEncoder.INPUT_FORMAT_RGB, stride : 9,

			pixels : [
				255,0,0,  255,0,0, 255,0,0,
				0,255,0,  0,255,0, 0,255,0,
				0,0,255,  0,0,255, 0,0,255,
				]
		}

		],
	expected : "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAMAAAADCAIAAADZSiLoAAAAE0lEQVQImWP8zwAFjAz/4SwYEwA2JAMBWInC2QAAAABJRU5ErkJggg=="
};

var png1B = {
	// A 3x3 image, rows are red, green, blue.
	// RGB format, transparency=none.

	transparency : "none",

	frames  : [
		{
			width  : 3,    height : 3,

			format : Ci.imgIEncoder.INPUT_FORMAT_RGB, stride : 9,

			pixels : [
				255,0,0,  255,0,0, 255,0,0,
				0,255,0,  0,255,0, 0,255,0,
				0,0,255,  0,0,255, 0,0,255,
				]
		}

		],
	expected : "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAMAAAADCAIAAADZSiLoAAAAE0lEQVQImWP8zwAFjAz/4SwYEwA2JAMBWInC2QAAAABJRU5ErkJggg=="
};

var png2A = {
	// A 3x3 image, rows are: red, green, blue. Columns are: 0%, 33%, 66% transparent.

	transparency : null,

	frames  : [
		{
			width  : 3,    height : 3,

			format : Ci.imgIEncoder.INPUT_FORMAT_RGBA, stride : 12,

			pixels : [
				255,0,0,255,  255,0,0,170, 255,0,0,85,
				0,255,0,255,  0,255,0,170, 0,255,0,85,
				0,0,255,255,  0,0,255,170, 0,0,255,85
				]
		}

		],
	expected : "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAMAAAADCAYAAABWKLW/AAAAH0lEQVQImWP8z8Dwn4GBYTUDA8NqFsb/DHDAwoDEAwCSYQVewgzM5AAAAABJRU5ErkJggg=="
};

var png2B = {
	// A 3x3 image, rows are: red, green, blue. Coulmns are: 0%, 33%, 66% transparent,
	// but transparency will be ignored.

	transparency : "none",

	frames  : [
		{
			width  : 3,    height : 3,

			format : Ci.imgIEncoder.INPUT_FORMAT_RGBA, stride : 12,

			pixels : [
				255,0,0,255,  255,0,0,170, 255,0,0,85,
				0,255,0,255,  0,255,0,170, 0,255,0,85,
				0,0,255,255,  0,0,255,170, 0,0,255,85
				]
		}

		],
	expected : "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAMAAAADCAIAAADZSiLoAAAAE0lEQVQImWP8zwAFjAz/4SwYEwA2JAMBWInC2QAAAABJRU5ErkJggg=="
};


// Main test entry point.
function run_test() {
	dump("Checking png1A...\n")
	run_test_for(png1A);
	dump("Checking png1B...\n")
	run_test_for(png1B);
	dump("Checking png2A...\n")
	run_test_for(png2A);
	dump("Checking png2B...\n")
	run_test_for(png2B);
}; 


function run_test_for(input) {
	var encoder, dataURL;

	encoder = encodeImage(input);
	dataURL = makeDataURL(encoder, "image/png");
	do_check_eq(dataURL, input.expected);
};


function encodeImage(input) {
	var encoder = Cc["@mozilla.org/image/encoder;2?type=image/png"].createInstance();
	encoder.QueryInterface(Ci.imgIEncoder);

	var options = "";
	if (input.transparency) {
		options += "transparency=" + input.transparency;
	}

	var frame = input.frames[0];
	encoder.initFromData(frame.pixels, frame.pixels.length,
				frame.width, frame.height, frame.stride,
				frame.format, options);
	return encoder;
}


function makeDataURL(encoder, mimetype) {
	var rawStream = encoder.QueryInterface(Ci.nsIInputStream);

	var stream = Cc["@mozilla.org/binaryinputstream;1"].createInstance();
	stream.QueryInterface(Ci.nsIBinaryInputStream);

	stream.setInputStream(rawStream);

	var bytes = stream.readByteArray(stream.available()); // returns int[]

	var base64String = toBase64(bytes);

	return "data:" + mimetype + ";base64," + base64String;
}

/* toBase64 copied from extensions/xml-rpc/src/nsXmlRpcClient.js */

/* Convert data (an array of integers) to a Base64 string. */
const toBase64Table = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz' +
    '0123456789+/';
const base64Pad = '=';
function toBase64(data) {
    var result = '';
    var length = data.length;
    var i;
    // Convert every three bytes to 4 ascii characters.
    for (i = 0; i < (length - 2); i += 3) {
        result += toBase64Table[data[i] >> 2];
        result += toBase64Table[((data[i] & 0x03) << 4) + (data[i+1] >> 4)];
        result += toBase64Table[((data[i+1] & 0x0f) << 2) + (data[i+2] >> 6)];
        result += toBase64Table[data[i+2] & 0x3f];
    }

    // Convert the remaining 1 or 2 bytes, pad out to 4 characters.
    if (length%3) {
        i = length - (length%3);
        result += toBase64Table[data[i] >> 2];
        if ((length%3) == 2) {
            result += toBase64Table[((data[i] & 0x03) << 4) + (data[i+1] >> 4)];
            result += toBase64Table[(data[i+1] & 0x0f) << 2];
            result += base64Pad;
        } else {
            result += toBase64Table[(data[i] & 0x03) << 4];
            result += base64Pad + base64Pad;
        }
    }

    return result;
}
