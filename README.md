# AKA

A digital contact book for linux.

## Install 

To compile from source, first you need the following dependencies:

`gtk+-3.0`

`sqlite3`

`pcre` (specifically version 1, not 2)

`git`

`make`

`cmake` (version 3.10 or higher)

To build from source, run the following:

```sh
mkdir build
cd build
cmake ../src
make
```

And then the binary will be compiled into the **build** directory.

## Instructions

This is for the FBLA 2024 Coding and Programming project.
Please see **fbla/info.md** for more information.
Partners can be stored as Contacts in this application,
and the 25 included partners (who have been made up)
are in the **example.db** file.

Below are extra guides that are also available in the pop up,
after the **Help** menu item is clicked:

[Creating a Contact](#creating-a-contact)

[Editing a Contact](#editing-a-contact)

[Deleting a Contact](#deleting-a-contact)

[Importing a Save](#importing-a-save)

## Creating a Contact

To create a **Contact**, you can click the **New Contact** button.

After that, the **New Contact View** will pop up in the **Main View**,
and you can enter your **Contact** into the **Entry Bars**.

After you are done entering your **Contact** you can press the **Save Button**
and the **Contact** will be saved.

Additionally, to open the **New Contact View** you can click on the **Edit** menu,
and click the **New Contact** item.

## Editing a Contact

To edit a **Contact**, select it in the **List**,
and then click on the **Edit Contact** button in the **Main View**.

After clicking the **Edit** button, 
the **Edit Contact View** will appear in the **Main View**,
and the **Contact** can be edited in the **Entry Bars**,
which have the previous **Contact** loaded into them.

When you press the **Save Button** at the bottom of the **Main View**,
the current values inside of the **Entry Bars** will be saved. 

Additionally, to edit a contact, you can click on the **Edit** Menu,
and then click the **Edit Contact** item,
or you can click on the **Edit Contact** button in the **Action Box**.

## Deleting a Contact

To delete a **Contact**, first select the **Contact** in the **List**. 

Then, mouse down to where you can see the **Delete Contact** Button,
and delete the **Contact**.

Additionally, after a **Contact** is selected on the **List**, 
you can click on the **Edit** menu,
and click the **Delete Contact** item.

## Backing up a Save

To back up a **Save**, click on the **File** menu,
then on the **Backup Contacts** item,
and then navigate to where you would like to save your **Contacts**.

The **File Name** must end in **.db** or it will not save.

## Opening a Save

To open a **save**, click on the **File** menu,
click on the **Open Contacts** item,
and navigate to the save that you would like to open.

**Saves** must have a **.db** file ending,
and they can be created by using **Backup Contacts** in the
**File** menu.
