#!/usr/bin/env bash
for i in $1*; do
  page_path="$i"
  page_name="$i"
  echo "<div><a href=\"$page_path\">$page_name</a></div>" >> links.html
done
mv links.html www/public/html/links.html