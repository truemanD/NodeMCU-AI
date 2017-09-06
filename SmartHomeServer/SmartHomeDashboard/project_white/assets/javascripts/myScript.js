var http = require('http'),
        var fs = require('fs');
        http.createServer(function(request, response) {

        fs.readFile('HtmlPage.html', function (err, html) {
        if (err) {
        throw err;
        }

        response.writeHeader(200, {"Content-Type": "text/html"});
                response.write(html);
                response.end();
        });
                }).listen(8000); });