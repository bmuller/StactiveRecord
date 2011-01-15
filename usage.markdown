---
layout: base
title: StactiveRecord - C++ ORM
---
# Simple Example Explained 
A full description of the simple example is given [here](exampledescription.html).

# Database Connections 
PostgreSQL, MySQL, and SQLite are supported.  You must set the static variable **Sar_Dbi::dbi** in the namespace where you are using **StactiveRecord**.  This is done using the **makeStorage** static function.  The single string argument has the configuration:
{% highlight mysql %}
scheme://[user[:password]@host[:port]/]database
{% endhighlight %}
scheme is one of **posgres**, **mysql**, or **sqlite**.  The easiest way to start playing is to just use a database in memory using sqlite with the following option: 
{% highlight cpp %}
Sar_Dbi * Sar_Dbi::dbi = Sar_Dbi::makeStorage("sqlite://:memory:");
{% endhighlight %}

There is an alternative second parameter which specifies a table name prefix.  This can be used if you only have one database and want to single the Stactive Record tables out from the rest.
{% highlight cpp %}

// All tables used by Stactive Record will beging with "stactive_record_"
Sar_Dbi * Sar_Dbi::dbi = Sar_Dbi::makeStorage("postgres://user:password@localhost/myonlydatabase", "stactive_record_");
{% endhighlight %}

# Object Relationship Examples 
 * [One To One](onetoone.html)
 * [One To Many](onetomany.html)
 * [Many To Many](manytomany.html)

# Queries 
Queries are represented by *Q* objects and can be chained using the binary operators *&&* and *||* (for AND and OR, respectively).  They are constructed with the name of a property and a description of what values/value ranges you're looking for (with the exception of *hasobject*, as described later).  For example, 
{% highlight cpp %}
(Q("age", between(18, 20)) || Q("age", between(22, 30))) && Q("name", "Cool Hand Luke")
{% endhighlight %}
would signify that you're looking for an object with with an age between 18 and 20, or between 22 and 30 AND with a name of *Cool Hand Luke*.  *Q* objects are passed to your Record object's find method which returns and *!ObjGroup* (a vector with some special additional methods).  For example:
{% highlight cpp %}
 ObjGroup<Person> people = Person::find(Q("age", between(40, 100)) && Q("fullname", startswith("Robert")));
{% endhighlight %}

In the case that you're looking for a object that has a certain relation, you would use:
{% highlight cpp %}
// Assuming that *address* is an instance of a class **Address** and that some people have the same address set
ObjGroup<Person> people = Person::find(Q(hasobject(address)));
{% endhighlight %}

The following value descriptions are available:
 * startswith(std::string value)
 * endswith(std::string value)
 * contains(std::string value)
 * lessthan(int value)
 * greaterthan(int value)
 * between(int value, int valuetwo)
 * equals(int value)
 * lessthan(!DateTime value)
 * greaterthan(!DateTime value)
 * between(DateTime value, DateTime valuetwo)
 * equals(DateTime value)
 * equals(std::string value)
 * equals(bool value)
 * in(std::vector&lt;int&gt; values)
 * isnull()
 * hasobject(Record&lt;T&gt;&amp; r)

In addition, each of these can be negated:
 * nstartswith(std::string value)
 * nendswith(std::string value)
 * ncontains(std::string value)
 * nlessthan(int value)
 * ngreaterthan(int value)
 * nbetween(int value, int valuetwo)
 * nequals(int value)
 * nlessthan(DateTime value)
 * ngreaterthan(DateTime value)
 * nbetween(DateTime value, DateTime valuetwo)
 * nequals(DateTime value)
 * nequals(std::string value)
 * nequals(bool value)
 * nin(std::vector&lt;int&gt; values)
 * nisnull()

You can take a look at the queries example file for more examples (this file is included in the examples directory).
