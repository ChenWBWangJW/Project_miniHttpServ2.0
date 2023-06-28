<html lang=\"zh-CN\">
<head>
<meta content=\"text/html; charset=utf-8\" http-equiv=\"content-Type\">
<title>This is a test</title>
</head>
<body>
<dir align=center height=\"500px\">
<br/><br/><br/>
<h2>Please enter the information and log in </h2><br/><br/>
<form action="bin_cgi/test.cgi" method="post">
Name: <input type="text" name="name" />
<br/>Age: <input type="password" name="age" />
<br/><br/><br/><input type="submit" value="Submit" />
<input type="reset" value="Reset" />
</form>
</div>
</body>
</html>