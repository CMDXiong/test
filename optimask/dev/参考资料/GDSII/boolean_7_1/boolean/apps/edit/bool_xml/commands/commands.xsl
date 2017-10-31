<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
               version='1.0'>

<xsl:template match="/">
	<html>
        <xsl:apply-templates/>
	</html>
</xsl:template>

<xsl:template match="commands">
   	<head>
    </head>
        <xsl:apply-templates/>
    <body>
   	</body>
</xsl:template>

<xsl:template match="command">
    <h3>Command:</h3>
	<p>
        name:
        <xsl:apply-templates select="name"/>
	</p>
	<p>
        syntax:
        <xsl:apply-templates select="syntax"/>
	<p>
	</p>
        <xsl:apply-templates select="short"/>
        <xsl:apply-templates/>
	</p>
</xsl:template>

</xsl:stylesheet>

